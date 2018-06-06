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
//Definition of escrow objects
class escrow_object : public graphene::db::abstract_object<escrow_object>
{
  public:
    static const uint8_t space_id = implementation_ids;
    static const uint8_t type_id = impl_escrow_object_type;

    uint32_t escrow_id; //Number to identify the escrow object.
    // === WHAT SHOULD BE THE DATA TYPE=== //
    account_id_type from;  //ESCROW sender
    account_id_type to;    //ESCROW receiver
    account_id_type agent; // the TRUSTED ESCROW entity
    time_point_sec expiration;
    asset balance;         // how many assets to hold on escrow
    bool disputed = false; //dispute flag

    /*--- COMMENTED FOR NOW --- */
    // bool agent_approved = false; // did agent approve the transaction?
    // bool receiver_approved = false; // did receiver approve the transaction?
};

struct by_from_id;
struct by_to;
struct by_agent;

// creating object index
typedef multi_index_container<
    escrow_object,
    indexed_by<
        ordered_unique<tag<by_id>, member<object, object_id_type, &object::id>>,
        ordered_unique<tag<by_from_id>,
                       composite_key<escrow_object,
                                     member<escrow_object, account_id_type, &escrow_object::from>,
                                     member<escrow_object, uint32_t, &escrow_object::escrow_id>>>,
        ordered_unique<tag<by_to>,
                       composite_key<escrow_object,
                                     member<escrow_object, account_id_type, &escrow_object::to>,
                                     member<object, object_id_type, &object::id>>>,
        ordered_unique<tag<by_agent>,
                       composite_key<escrow_object,
                                     member<escrow_object, account_id_type, &escrow_object::agent>,
                                     member<object, object_id_type, &object::id>>>>>
    escrow_object_index_type;

typedef generic_index<escrow_object, escrow_object_index_type> escrow_index;

} // namespace chain
} // namespace graphene

FC_REFLECT_DERIVED(graphene::chain::escrow_object, (graphene::db::object),
                   (escrow_id)(from)(to)(agent)(expiration)(balance)(disputed));
