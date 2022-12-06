#include "SampleRateModel.h"
#include "tensors/SymSymR4.h"

SampleRateModel::SampleRateModel(const std::string & name)
  : Model(name)
{
  input().add<LabeledAxis>("state");
  input().subaxis("state").add<Scalar>("foo");
  input().subaxis("state").add<Scalar>("bar");
  input().subaxis("state").add<SymR2>("baz");

  input().add<LabeledAxis>("forces");
  input().subaxis("forces").add<Scalar>("temperature");

  output().add<LabeledAxis>("state");
  output().subaxis("state").add<Scalar>("foo_rate");
  output().subaxis("state").add<Scalar>("bar_rate");
  output().subaxis("state").add<SymR2>("baz_rate");

  setup();
}

void
SampleRateModel::set_value(LabeledVector in, LabeledVector out, LabeledMatrix * dout_din) const
{
  // Grab the trial states
  auto foo = in.slice(0, "state").get<Scalar>("foo");
  auto bar = in.slice(0, "state").get<Scalar>("bar");
  auto baz = in.slice(0, "state").get<SymR2>("baz");

  // Say the rates depend on temperature, for fun
  auto T = in.slice(0, "forces").get<Scalar>("temperature");

  // Some made up rates
  auto foo_dot = (foo * foo + bar) * T + baz.tr();
  auto bar_dot = -bar / 100 - 0.5 * foo - 0.9 * T + baz.tr();
  auto baz_dot = (foo + bar) * baz * (T - 3);

  // Set the output
  out.slice(0, "state").set(foo_dot, "foo_rate");
  out.slice(0, "state").set(bar_dot, "bar_rate");
  out.slice(0, "state").set(baz_dot, "baz_rate");

  if (dout_din)
  {
    TorchSize nbatch = in.batch_size();
    auto dfoo_dot_dfoo = 2 * foo * T;
    auto dfoo_dot_dbar = T;
    auto dfoo_dot_dbaz = SymR2::identity().expand_batch(nbatch);
    auto dbar_dot_dfoo = Scalar(-0.5, nbatch);
    auto dbar_dot_dbar = Scalar(-0.01, nbatch);
    auto dbar_dot_dbaz = SymR2::identity().expand_batch(nbatch);
    auto dbaz_dot_dfoo = baz * (T - 3);
    auto dbaz_dot_dbar = baz * (T - 3);
    auto dbaz_dot_dbaz = (foo + bar) * (T - 3) * SymR2::identity_map().expand_batch(nbatch);

    dout_din->block("state", "state").set(dfoo_dot_dfoo, "foo_rate", "foo");
    dout_din->block("state", "state").set(dfoo_dot_dbar, "foo_rate", "bar");
    dout_din->block("state", "state").set(dfoo_dot_dbaz, "foo_rate", "baz");
    dout_din->block("state", "state").set(dbar_dot_dfoo, "bar_rate", "foo");
    dout_din->block("state", "state").set(dbar_dot_dbar, "bar_rate", "bar");
    dout_din->block("state", "state").set(dfoo_dot_dbaz, "bar_rate", "baz");
    dout_din->block("state", "state").set(dbaz_dot_dfoo, "baz_rate", "foo");
    dout_din->block("state", "state").set(dbaz_dot_dbar, "baz_rate", "bar");
    dout_din->block("state", "state").set(dbaz_dot_dbaz, "baz_rate", "baz");

    auto dfoo_dot_dT = foo * foo + bar;
    auto dbar_dot_dT = Scalar(-0.9, nbatch);
    auto dbaz_dot_dT = (foo + bar) * baz;

    dout_din->block("state", "forces").set(dfoo_dot_dT, "foo_rate", "temperature");
    dout_din->block("state", "forces").set(dbar_dot_dT, "bar_rate", "temperature");
    dout_din->block("state", "forces").set(dbaz_dot_dT, "baz_rate", "temperature");
  }
}
