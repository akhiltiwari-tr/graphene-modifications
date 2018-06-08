#pragma once

#include <string>

#include <graphene/db/object.hpp>
#include <graphene/db/generic_index.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <boost/multi_index_container_fwd.hpp>

using std::string;

namespace graphene
{
namespace chain
{
/**
*Definition of escrow objects
** Temporally save escrow transactions until funds are released or operation expired.
*/
class escrow_object : public graphene::db::abstract_object<escrow_object>
{
  public:
    static const uint8_t space_id = implementation_ids;
    static const uint8_t type_id = impl_escrow_object_type;

    uint32_t escrow_id = 10; //Number to identify the escrow object.
    // === WHAT SHOULD BE THE DATA TYPE=== //
    account_id_type from;  //ESCROW sender
    account_id_type to;    //ESCROW receiver
    account_id_type agent; // the TRUSTED ESCROW entity
    asset amount;          // how many assets to hold on escrow
    time_point_sec ratification_deadline;
    time_point_sec escrow_expiration;
    asset pending_fee;
    bool disputed = false; //dispute flag
    bool to_approved = false;
    bool agent_approved = false;

    bool is_approved() const { return to_approved && agent_approved; }
};

struct by_from_id;
struct by_ratification_deadline;

// creating object index
typedef multi_index_container<
    escrow_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,

        ordered_unique<tag<by_from_id>,
                       composite_key<escrow_object,
                                     member<escrow_object, account_id_type, &escrow_object::from>,
                                     member<escrow_object, uint32_t, &escrow_object::escrow_id>>>,
        ordered_unique<tag<by_ratification_deadline>,
                       composite_key<escrow_object,
                                     const_mem_fun<escrow_object, bool, &escrow_object::is_approved>,
                                     member<escrow_object, time_point_sec, &escrow_object::ratification_deadline>,
                                     member<escrow_object, uint32_t, &escrow_object::escrow_id>>,
                       composite_key_compare<std::less<bool>, std::less<time_point_sec>, std::less<uint32_t>>>>>
    escrow_object_index_type;

typedef generic_index<escrow_object, escrow_object_index_type> escrow_index;

} // namespace chain
} // namespace graphene

FC_REFLECT_DERIVED(graphene::chain::escrow_object, (graphene::db::object),
                   (escrow_id)(from)(to)(agent)(ratification_deadline)(escrow_expiration)(pending_fee)(amount)(disputed)(to_approved)(agent_approved));
