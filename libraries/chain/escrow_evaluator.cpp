#include <graphene/chain/database.hpp>
#include <graphene/chain/escrow_evaluator.hpp>
#include <graphene/chain/escrow_object.hpp>

namespace graphene
{
namespace chain
{

void_result escrow_transfer_evaluator::do_evaluate(const escrow_transfer_operation &o)
{
}

object_id_type escrow_transfer_evaluator::do_apply(const escrow_transfer_operation &o)
{
    try
    {
        FC_ASSERT(db().get_balance(o.from, o.amount.asset_id) >= (o.amount + o.fee));

        if (o.fee.amount > 0)
        {
            db().adjust_balance(o.from, -o.fee);
            db().adjust_balance(o.agent, o.fee);
        }

        db().adjust_balance(o.from, -o.amount);

        db().create<escrow_object>([&](escrow_object &esc) {
            esc.escrow_id = o.escrow_id;
            esc.from = o.from;
            esc.to = o.to;
            esc.agent = o.agent;
            esc.balance = o.amount;
            esc.expiration = o.expiration;
        });
    }
    FC_CAPTURE_AND_RETHROW((o))
}

void_result escrow_dispute_evaluator::do_evaluate(const escrow_dispute_operation &o)
{
}

object_id_type escrow_dispute_evaluator::do_apply(const escrow_dispute_operation &o)
{
    try
    {
        const auto &e = db().get_escrow(o.from, o.escrow_id);
        FC_ASSERT(!e.disputed);
        FC_ASSERT(e.to == o.to);

        db().modify(e, [&](escrow_object &esc) {
            esc.disputed = true;
        });
    }
    FC_CAPTURE_AND_RETHROW((o))
}

void_result escrow_release_evaluator::do_evaluate(const escrow_release_operation &o)
{
}

object_id_type escrow_release_evaluator::do_apply(const escrow_release_operation &o)
{
    try
    {
        const auto &e = db().get_escrow(o.from, o.escrow_id);
        FC_ASSERT(e.balance >= o.amount && e.balance.asset_id == o.amount.asset_id);

        if (o.who == e.from)
            FC_ASSERT(o.to == e.to);
        else if (o.who == e.to)
            FC_ASSERT(o.to == e.from);
        else
        {
            FC_ASSERT(e.disputed && o.who == e.agent);
        }

        db().adjust_balance(o.to, o.amount);
        if (e.balance == o.amount)
            db().remove(e);
        else
        {
            db().modify(e, [&](escrow_object &esc) {
                esc.balance -= o.amount;
            });
        }
    }
    FC_CAPTURE_AND_RETHROW((o))
}

} // namespace chain
} // namespace graphene
