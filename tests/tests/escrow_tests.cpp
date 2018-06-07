#include <boost/test/unit_test.hpp>

#include <graphene/chain/database.hpp>
#include <graphene/chain/hardfork.hpp>

#include <graphene/chain/balance_object.hpp>
//#include <graphene/chain/budget_record_object.hpp>
//#include <graphene/chain/committee_member_object.hpp>
//#include <graphene/chain/market_object.hpp>
//#include <graphene/chain/withdraw_permission_object.hpp>
//#include <graphene/chain/witness_object.hpp>
//#include <graphene/chain/worker_object.hpp>
#include <graphene/chain/escrow_object.hpp>

//#include <graphene/utilities/tempdir.hpp>

//#include <fc/crypto/digest.hpp>

#include "../common/database_fixture.hpp"

using namespace graphene::chain;
using namespace graphene::chain::test;

BOOST_FIXTURE_TEST_SUITE(escrow_tests, database_fixture)

BOOST_AUTO_TEST_CASE(escrow_transfer)
{
    try
    {
        ACTORS((alice)(bob)(sam));

        //enable_fees();
        transfer(committee_account, alice_id, asset(100000000));

        BOOST_REQUIRE_EQUAL(get_balance(alice_id, asset_id_type()), 100000000);
        BOOST_REQUIRE_EQUAL(get_balance(bob_id, asset_id_type()), 0);
        BOOST_REQUIRE_EQUAL(get_balance(sam_id, asset_id_type()), 0);

        const asset_object &core = asset_id_type()(db);

        BOOST_TEST_MESSAGE("Testing: escrow_transfer creating object");
        {
            escrow_transfer_operation op;
            op.from = alice_id;
            op.to = bob_id;
            op.amount = core.amount(1000);
            op.escrow_id = 0;
            op.agent = sam_id;
            op.fee = asset(100, asset_id_type());
            op.json_meta = "";
            //op.ratification_deadline = db.head_block_time() + 100;
            //op.escrow_expiration = db.head_block_time() + 200;
            trx.operations.push_back(op);
            sign(trx, alice_private_key);
            PUSH_TX(db, trx);
            generate_block();
            trx.clear();
        }

        escrow_object escrow = db.get_escrow(alice_id, 0);

        BOOST_REQUIRE(escrow.from == alice_id);
        BOOST_REQUIRE(escrow.to == bob_id);
        BOOST_REQUIRE(escrow.escrow_id == 0);
        BOOST_REQUIRE(escrow.agent == sam_id);
        BOOST_REQUIRE(escrow.disputed == false);
        BOOST_REQUIRE(escrow.to_approved == false);
        BOOST_REQUIRE(escrow.agent_approved == false);

        BOOST_REQUIRE_EQUAL(get_balance(alice_id, asset_id_type()), 99998900);
        BOOST_REQUIRE_EQUAL(get_balance(bob_id, asset_id_type()), 0);
        BOOST_REQUIRE_EQUAL(get_balance(sam_id, asset_id_type()), 0);

        // agent approves but also need alice key, this is wrong
        BOOST_TEST_MESSAGE("Testing: escrow release");
        {
            escrow_approve_operation op;
            op.from = alice_id;
            op.to = bob_id;
            op.who = sam_id;
            op.escrow_id = 0;
            op.agent = sam_id;
            op.approve = true;
            trx.operations.push_back(op);
            sign(trx, sam_private_key);
            //sign(trx, alice_private_key);
            PUSH_TX(db, trx);
            generate_block();
            trx.clear();
        }

        BOOST_REQUIRE_EQUAL(get_balance(alice_id, asset_id_type()), 99998900);
        BOOST_REQUIRE_EQUAL(get_balance(bob_id, asset_id_type()), 0);
        BOOST_REQUIRE_EQUAL(get_balance(sam_id, asset_id_type()), 0);

        escrow = db.get_escrow(alice_id, 0);
        // escrow object still there, flags are changing
        BOOST_REQUIRE(escrow.from == alice_id);
        BOOST_REQUIRE(escrow.to == bob_id);
        BOOST_REQUIRE(escrow.escrow_id == 0);
        BOOST_REQUIRE(escrow.agent == sam_id);
        BOOST_REQUIRE(escrow.disputed == false);
        BOOST_REQUIRE(escrow.to_approved == false);
        BOOST_REQUIRE(escrow.agent_approved == true);

        // bob(to) approves. need alice sign, this is nonsense!
        {
            escrow_approve_operation op;
            op.from = alice_id;
            op.to = bob_id;
            op.who = bob_id;
            op.escrow_id = 0;
            op.agent = sam_id;
            op.approve = true;
            trx.operations.push_back(op);
            sign(trx, bob_private_key);
            //sign(trx, alice_private_key);
            PUSH_TX(db, trx);
            generate_block();
            trx.clear();
        }
        BOOST_REQUIRE_EQUAL(get_balance(alice_id, asset_id_type()), 99998900);
        BOOST_REQUIRE_EQUAL(get_balance(bob_id, asset_id_type()), 0);
        BOOST_REQUIRE_EQUAL(get_balance(sam_id, asset_id_type()), 0);

        escrow = db.get_escrow(alice_id, 0);
        BOOST_REQUIRE(escrow.from == alice_id);
        BOOST_REQUIRE(escrow.to == bob_id);
        BOOST_REQUIRE(escrow.escrow_id == 0);
        BOOST_REQUIRE(escrow.agent == sam_id);
        BOOST_REQUIRE(escrow.disputed == false);
        BOOST_REQUIRE(escrow.to_approved == true);
        BOOST_REQUIRE(escrow.agent_approved == true);

        // now the escrow haves all the needed aprovals release the funds with bob
        // still need alice sign, this haves to be fixed
        {
            escrow_release_operation op;

            op.from = alice_id;
            op.to = bob_id;
            op.who = bob_id;
            op.escrow_id = 0;
            op.amount = core.amount(1000);
            trx.operations.push_back(op);
            //sign(trx, alice_private_key);
            sign(trx, bob_private_key);
            PUSH_TX(db, trx);
            generate_block();
            trx.clear();
        }
        BOOST_REQUIRE_EQUAL(get_balance(alice_id, asset_id_type()), 99998900);
        BOOST_REQUIRE_EQUAL(get_balance(bob_id, asset_id_type()), 1000);
        BOOST_REQUIRE_EQUAL(get_balance(sam_id, asset_id_type()), 0);

        ///escrow object is deleted
        GRAPHENE_CHECK_THROW(db.get_escrow(alice_id, 0), fc::assert_exception);

        // perfecto this is the first scenario where no dispute and everybody approves everything.
    }
    FC_LOG_AND_RETHROW()
}

BOOST_AUTO_TEST_CASE(escrow_dispute)
{
}

BOOST_AUTO_TEST_CASE(escrow_authorities)
{
    try
    {
        ACTORS((alice)(bob)(sam));

        // trying approve now
        escrow_approve_operation op2;

        op2.from = alice_id;
        op2.to = bob_id;
        op2.agent = sam_id;
        op2.who = bob_id;
        op2.escrow_id = 0;
        op2.approve = true;

        //generate_block();

        flat_set<account_id_type> auths2;
        flat_set<account_id_type> expected2;

        op2.get_required_active_authorities(auths2);
        wdump((auths2));

        vector<authority> other;
        flat_set<account_id_type> active_set, owner_set;
        operation_get_required_authorities(op2, active_set, owner_set, other);

        wdump((active_set));
        wdump((owner_set));
        wdump((other));

        // executing the op

        // executing the operation
        set_expiration(db, trx);
        trx.clear();
        trx.operations.push_back(op2);

        //auto test = tx_missing_active_auth;
        //wdump((test));

        trx.signatures.clear();
        sign(trx, bob_private_key);
        wdump((trx));
        PUSH_TX(db, trx, 0);
        generate_block();
        set_expiration(db, trx);
    }
    FC_LOG_AND_RETHROW()
}
BOOST_AUTO_TEST_SUITE_END()