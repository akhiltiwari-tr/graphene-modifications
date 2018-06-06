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
}

void_result escrow_dispute_evaluator::do_evaluate(const escrow_dispute_operation &o)
{
}

object_id_type escrow_dispute_evaluator::do_apply(const escrow_dispute_operation &o)
{
}

void_result escrow_release_evaluator::do_evaluate(const escrow_release_operation &o)
{
}

object_id_type escrow_release_evaluator::do_apply(const escrow_release_operation &o)
{
}

} // namespace chain
} // namespace graphene
