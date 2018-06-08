#include <graphene/chain/protocol/escrow.hpp>

// Definition of the escrow.

namespace graphene
{
namespace chain
{
// ecsrow operation declaration.
void escrow_transfer_operation::validate() const
{
    FC_ASSERT(agent_fee.amount >= 0);
    FC_ASSERT(amount.amount >= 0);
    FC_ASSERT(from != to);
    FC_ASSERT(from != agent && to != agent);
    FC_ASSERT(agent_fee.asset_id == amount.asset_id); // agent fee; only in bts (temp)
    FC_ASSERT(amount.asset_id == asset_id_type());    // only bts is allowed for now. (temp)
    FC_ASSERT(fee.asset_id == asset_id_type());       // fee only in bts (temp)
}

void escrow_approve_operation::validate() const
{
    FC_ASSERT(who == to || who == agent);
    FC_ASSERT(fee.asset_id == asset_id_type()); // fee only in bts (temp)
}
void escrow_dispute_operation::validate() const
{
    FC_ASSERT(who == from || who == to);
    FC_ASSERT(fee.asset_id == asset_id_type()); // fee only in bts (temp)
}
void escrow_release_operation::validate() const
{
    FC_ASSERT(who == from || who == to || who == agent, "who must be from or to or agent");
    FC_ASSERT(receiver == from || receiver == to, "receiver must be from or to");
    FC_ASSERT(amount.amount >= 0);
    FC_ASSERT(amount.asset_id == asset_id_type()); // only bts is allowed for now (temp)
    FC_ASSERT(fee.asset_id == asset_id_type());    // fee only in bts (temp)
}
/*
      void escrow_transfer_operation::get_required_active_authorities( flat_set<account_id_type>& a )const
      { a.insert( from ); }
      void escrow_dispute_operation::get_required_owner_authorities( flat_set<account_id_type>& a )const
      { a.insert( from ); }
      void escrow_release_operation::get_required_active_authorities( flat_set<account_id_type>& a )const
      { a.insert( from ); }
*/
} // namespace chain
} // namespace graphene