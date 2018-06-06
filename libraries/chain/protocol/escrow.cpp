#include <graphene/chain/protocol/escrow.hpp>

// Definition of the escrow.

namespace graphene
{
namespace chain
{
// ecsrow operation declaration.
void escrow_transfer_operation::validate() const
{
    //FC_ASSERT( is_valid_account_name( from ) );
    //FC_ASSERT( is_valid_account_name( to ) );
    //FC_ASSERT( is_valid_account_name( agent ) );
    //FC_ASSERT( fee.amount >= 0 );
    //FC_ASSERT( amount.amount >= 0 );
    //FC_ASSERT( from != agent && to != agent );
    //FC_ASSERT( fee.symbol == amount.symbol );
    //FC_ASSERT( amount.symbol != VESTS_SYMBOL );
}
void escrow_dispute_operation::validate() const
{
    //FC_ASSERT( is_valid_account_name( from ) );
    //FC_ASSERT( is_valid_account_name( to ) );
    //FC_ASSERT( is_valid_account_name( who ) );
    //FC_ASSERT( who == from || who == to );
}
void escrow_release_operation::validate() const
{
    //FC_ASSERT( is_valid_account_name( from ) );
    //FC_ASSERT( is_valid_account_name( to ) );
    //FC_ASSERT( is_valid_account_name( who ) );
    //FC_ASSERT( who != to );
    //FC_ASSERT( amount.amount > 0 );
    //FC_ASSERT( amount.symbol != VESTS_SYMBOL );
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