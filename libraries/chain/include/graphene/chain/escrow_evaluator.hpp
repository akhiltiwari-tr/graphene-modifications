#pragma once
#include <graphene/chain/evaluator.hpp>

namespace graphene
{
namespace chain
{

class escrow_transfer_evaluator : public evaluator<escrow_transfer_evaluator>
{
public:
  typedef escrow_transfer_operation operation_type;

  void_result do_evaluate(const escrow_transfer_operation &o);
  object_id_type do_apply(const escrow_transfer_operation &o);
};

class escrow_dispute_evaluator : public evaluator<escrow_dispute_evaluator>
{
public:
  typedef escrow_dispute_operation operation_type;

  void_result do_evaluate(const escrow_dispute_operation &o);
  object_id_type do_apply(const escrow_dispute_operation &o);
};

class escrow_release_evaluator : public evaluator<escrow_release_evaluator>
{
public:
  typedef escrow_release_operation operation_type;

  void_result do_evaluate(const escrow_release_operation &o);
  object_id_type do_apply(const escrow_release_operation &o);
};

class escrow_approve_evaluator : public evaluator<escrow_approve_evaluator>
{
public:
  typedef escrow_approve_operation operation_type;

  void_result do_evaluate(const escrow_approve_operation &o);
  object_id_type do_apply(const escrow_approve_operation &o);
};

} // namespace chain
} // namespace graphene