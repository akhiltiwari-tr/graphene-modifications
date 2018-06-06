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
       *
       *  The escrow agent is paid the fee no matter what. It is up to the escrow agent to determine
       *
       *  Escrow transactions are uniquely identified by 'from' and 'escrow_id', the 'escrow_id' is defined
       *  by the sender.
       */

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
    string memo;

    uint32_t escrow_id;
    account_id_type agent;
    string json_meta;
    time_point_sec expiration;

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(from); }
    account_id_type fee_payer() const { return from; }
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
        uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    account_id_type to;
    uint32_t escrow_id;
    account_id_type who;

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(who); }
    account_id_type fee_payer() const { return from; }
};

/**
       *  This operation can be used by anyone associated with the escrow transfer to
       *  release funds if they have permission.
       **/
struct escrow_release_operation : public base_operation
{

    struct fee_parameters_type
    {
        uint64_t fee = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
    };
    asset fee;

    account_id_type from;
    uint32_t escrow_id;
    account_id_type to;  ///< the account that should receive funds (might be from, might be to
    account_id_type who; ///< the account that is attempting to release the funds, determines valid 'to'
    asset amount;        ///< the amount of funds to release

    void validate() const;
    void get_required_active_authorities(flat_set<account_id_type> &a) const { a.insert(who); }
    account_id_type fee_payer() const { return from; }
};

} // namespace chain
} // namespace graphene

FC_REFLECT(graphene::chain::escrow_transfer_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::escrow_dispute_operation::fee_parameters_type, (fee))
FC_REFLECT(graphene::chain::escrow_release_operation::fee_parameters_type, (fee))

FC_REFLECT(graphene::chain::escrow_transfer_operation, (from)(to)(amount)(memo)(escrow_id)(agent)(fee)(json_meta)(expiration));
FC_REFLECT(graphene::chain::escrow_dispute_operation, (from)(to)(escrow_id)(who));
FC_REFLECT(graphene::chain::escrow_release_operation, (from)(to)(escrow_id)(who)(amount));