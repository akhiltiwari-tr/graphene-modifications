#pragma once

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
*Definition of user  object for monetha account registration
** Save user data while account creation.
*/
class user_object : public graphene::db::abstract_object<user_object>
{
  public:
    static const uint8_t space_id = implementation_ids;
    static const uint8_t type_id = impl_user_object_type;

    uint32_t user_id = 0;         //Number to identify the user object.
    account_id_type user_account; //monetha user account
    string first_name;
    string last_name;
    string email;
    string phone_number;
    string address;
    float reputation;
};

// These structures are used in the multi_index_container....
struct by_user_account;

// creating object index
typedef multi_index_container<
    user_object, indexed_by<
                     ordered_unique<
                         tag<by_user_account>, member<object, object_id_type, &object::id>>>>
    user_object_index_type;
typedef generic_index<user_object, user_object_index_type> user_index;

} // namespace chain
} // namespace graphene

FC_REFLECT_DERIVED(graphene::chain::escrow_object, (graphene::db::object),
                   (user_id)(user_account)(first_name)(last_name)(email)(phone_number)(address)(reputation));
