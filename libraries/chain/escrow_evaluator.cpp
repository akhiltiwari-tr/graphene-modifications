#include <graphene/chain/database.hpp>
#include <graphene/chain/escrow_evaluator.hpp>
#include <graphene/chain/escrow_object.hpp>

namespace graphene
{
namespace chain
{

void_result escrow_transfer_evaluator::do_evaluate(const escrow_transfer_operation &o)
{
    FC_ASSERT(o.ratification_deadline > db().head_block_time());
    FC_ASSERT(o.escrow_expiration > db().head_block_time());
    FC_ASSERT(db().get_balance(o.from, o.amount.asset_id) >= (o.amount + o.fee + o.agent_fee));
    return void_result();
}

void_result escrow_transfer_evaluator::do_apply(const escrow_transfer_operation &o)
{
    try
    {
        if (o.agent_fee.amount > 0)
        {
            db().adjust_balance(o.from, -o.agent_fee);
            db().adjust_balance(o.agent, o.agent_fee);
        }

        db().adjust_balance(o.from, -o.amount);

        db().create<escrow_object>([&](escrow_object &esc) {
            esc.escrow_id = o.escrow_id;
            esc.from = o.from;
            esc.to = o.to;
            esc.agent = o.agent;
            esc.amount = o.amount;
            esc.pending_fee = o.agent_fee;
            esc.ratification_deadline = o.ratification_deadline;
            esc.escrow_expiration = o.escrow_expiration;
        });
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((o))
}

void_result escrow_approve_evaluator::do_evaluate(const escrow_approve_operation &o)
{
    const auto &escrow = db().get_escrow(o.from, o.escrow_id);
    FC_ASSERT(escrow.to == o.to, "op 'to' does not match escrow 'to'");
    FC_ASSERT(escrow.agent == o.agent, "op 'agent' does not match escrow 'agent'");
    FC_ASSERT(escrow.ratification_deadline >= db().head_block_time(), "escrow ratification deadline is before head block time");
    return void_result();
}

void_result escrow_approve_evaluator::do_apply(const escrow_approve_operation &o)
{

    try
    {
        const auto &escrow = db().get_escrow(o.from, o.escrow_id);

        // changing this to match.
        // const auto& escrow = db().get_escrow(o.to, o.escrow_id);
        bool reject_escrow = !o.approve;

        if (o.who == o.to)
        {
            FC_ASSERT(!escrow.to_approved, "'to' has already approved the escrow");

            if (!reject_escrow)
            {
                db().modify(escrow, [&](escrow_object &esc) {
                    esc.to_approved = true;
                });
            }
        }
        else if (o.who == o.agent)
        {
            FC_ASSERT(!escrow.agent_approved, "'agent' has already approved the escrow");

            if (!reject_escrow)
            {
                db().modify(escrow, [&](escrow_object &esc) {
                    esc.agent_approved = true;
                });
            }
        }
        else
        {
            FC_ASSERT(false, "op 'who' is not 'to' or 'agent'. This should have failed validation. Please create a github issue with this error dump.");
        }

        if (reject_escrow)
        {
            db().adjust_balance(o.from, escrow.amount);
            db().adjust_balance(o.from, escrow.pending_fee);

            db().remove(escrow);
        }
        else if (escrow.to_approved && escrow.agent_approved)
        {
            db().adjust_balance(o.agent, escrow.pending_fee);
            db().modify(escrow, [&](escrow_object &esc) {
                esc.pending_fee.amount = 0;
            });
        }
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((o))
}

void_result escrow_dispute_evaluator::do_evaluate(const escrow_dispute_operation &o)
{
    const auto &e = db().get_escrow(o.from, o.escrow_id);
    FC_ASSERT(e.to_approved && e.agent_approved, "escrow must be approved by all parties before a dispute can be raised");
    FC_ASSERT(!e.disputed, "escrow is already under dispute");
    FC_ASSERT(e.to == o.to, "op 'to' does not match escrow 'to'");
    FC_ASSERT(e.agent == o.agent, "op 'agent' does not match escrow 'agent'");
    return void_result();
}

void_result escrow_dispute_evaluator::do_apply(const escrow_dispute_operation &o)
{
    try
    {
        const auto &e = db().get_escrow(o.from, o.escrow_id);

        db().modify(e, [&](escrow_object &esc) {
            esc.disputed = true;
        });
        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((o))
}

void_result escrow_release_evaluator::do_evaluate(const escrow_release_operation &o)
{
    const auto &e = db().get_escrow(o.from, o.escrow_id);

    FC_ASSERT(e.amount >= o.amount && e.amount.asset_id == o.amount.asset_id);

    FC_ASSERT(o.amount.amount > 0 && e.amount.amount > 0);

    FC_ASSERT(e.to == o.to, "op 'to' does not match escrow 'to'");
    FC_ASSERT(e.agent == o.agent, "op 'agent' does not match escrow 'agent'");
    FC_ASSERT(o.receiver == e.from || o.receiver == e.to, "Funds must be released to 'from' or 'to'");

    FC_ASSERT(e.to_approved && e.agent_approved, "Funds cannot be released prior to escrow approval.");

    // If there is a dispute regardless of expiration, the agent can release funds to either party
    if (e.disputed)
    {
        FC_ASSERT(o.who == e.agent, "'agent' must release funds for a disputed escrow");
    }
    else
    {
        FC_ASSERT(o.who == e.from || o.who == e.to, "Only 'from' and 'to' can release from a non-disputed escrow");

        if (e.escrow_expiration > db().head_block_time())
        {
            // If there is no dispute and escrow has not expired, either party can release funds to the other.
            if (o.who == e.from)
            {
                FC_ASSERT(o.receiver == e.to, "'from' must release funds to 'to'");
            }
            else if (o.who == e.to)
            {
                FC_ASSERT(o.receiver == e.from, "'to' must release funds to 'from'");
            }
        }
    }

    return void_result();
}

void_result escrow_release_evaluator::do_apply(const escrow_release_operation &o)
{
    try
    {
        //FC_ASSERT( db().has_hardfork( STEEMIT_HARDFORK_0_9 ) ); /// TODO: remove this after HF9

        const auto &e = db().get_escrow(o.from, o.escrow_id);

        db().adjust_balance(o.receiver, o.amount);
        db().modify(e, [&](escrow_object &esc) {
            esc.amount -= o.amount;
        });

        if (e.amount.amount == 0)
        {
            db().remove(e);
        }

        return void_result();
    }
    FC_CAPTURE_AND_RETHROW((o))
}

} // namespace chain
} // namespace graphene
