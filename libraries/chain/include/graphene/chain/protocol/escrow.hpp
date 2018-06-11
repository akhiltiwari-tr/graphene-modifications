#pragma once

#include <graphene/chain/protocol/base.hpp>
// for header files in this directory, do remember to refer to README.MD

namespace graphene
{
namespace chain
{
/**
       *  The purpose of this operation is to enable someone to send money contingently to
       *  another individual. The funds leave the *from* account and go into a temporary balance
       *  where they are held until *from* releases it to *to*   or *to* refunds it to *from*.
       *
       *  In the event of a dispute the *agent* can divide the funds between the to/from account.
       *  Disputes can be raised any time before or on the dispute deadline time, after the escrow
       *  has been approved by all parties.
       *
       *  This operation only creates a proposed escrow transfer. Both the *agent* and *to* must
       *  agree to the terms of the arrangement by approving the escrow.
       *
       *  The escrow agent is paid the fee on approval of all parties. It is up to the escrow agent
       *  to determine the fee.
       *
       *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
       *  by the sender.
       **/

struct escrow_transfer_operation : public base_operation
{
    struct fee_parameters_type
    {
        uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    account_id_type to;
    asset amount;

    uint32_t escrow_id = 0;
    account_id_type agent;
    asset agent_fee;
    string json_meta;
    time_point_sec ratification_deadline;
    time_point_sec escrow_expiration;

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(from); }
    account_id_type fee_payer() const { return from; }
};

/**
       *  The agent and to accounts must approve an escrow transaction for it to be valid on
       *  the blockchain. Once a part approves the escrow, the cannot revoke their approval.
       *  Subsequent escrow approve operations, regardless of the approval, will be rejected.
      */
struct escrow_approve_operation : public base_operation
{
    struct fee_parameters_type
    {
        uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    account_id_type to;
    account_id_type agent;
    account_id_type who; // Either to or agent
    uint32_t escrow_id;
    bool approve;

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(who); }
    account_id_type fee_payer() const { return who; }
};

/**
       *  If either the sender or receiver of an escrow payment has an issue, they can
       *  raise it for dispute. Once a payment is in dispute, the agent has authority over
       *  who gets what.
      **/
struct escrow_dispute_operation : public base_operation
{
    struct fee_parameters_type
    {
        uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    account_id_type to;
    uint32_t escrow_id;
    account_id_type who;
    account_id_type agent;

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(who); }
    account_id_type fee_payer() const { return who; }
};

/**
       *  This operation can be used by anyone associated with the escrow transfer to
       *  release funds if they have permission.
       *
       *  The permission scheme is as follows:
       *  If there is no dispute and escrow has not expired, either party can release funds to the other.
       *  If escrow expires and there is no dispute, either party can release funds to either party.
       *  If there is a dispute regardless of expiration, the agent can release funds to either party
       *     following whichever agreement was in place between the parties.
       */
struct escrow_release_operation : public base_operation
{

    struct fee_parameters_type
    {
        uint64_t fee = 1 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    uint32_t escrow_id;
    account_id_type to;  ///< the account that should receive funds (might be from, might be to
    account_id_type who; ///< the account that is attempting to release the funds, determines valid 'to'
    account_id_type agent;
    account_id_type receiver; //< the account that should receive funds (might be from, might be to)
    asset amount;             ///< the amount of funds to release

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(who); }
    account_id_type fee_payer() const { return who; }
};

} // namespace chain
} // namespace graphene

FC_REFLECT(graphene::chain::escrow_transfer_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::escrow_approve_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::escrow_dispute_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::escrow_release_operation::fee_parameters_type, (fee))

FC_REFLECT(graphene::chain::escrow_transfer_operation, (fee)(from)(to)(agent)(amount)(escrow_id)(agent_fee)(json_meta)(ratification_deadline)(escrow_expiration));
FC_REFLECT(graphene::chain::escrow_approve_operation, (fee)(from)(to)(agent)(who)(escrow_id)(approve));
FC_REFLECT(graphene::chain::escrow_dispute_operation, (fee)(from)(to)(agent)(who)(escrow_id));
FC_REFLECT(graphene::chain::escrow_release_operation, (fee)(from)(to)(agent)(who)(receiver)(escrow_id)(amount));
