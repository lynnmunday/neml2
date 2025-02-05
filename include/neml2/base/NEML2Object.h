// Copyright 2023, UChicago Argonne, LLC
// All Rights Reserved
// Software Name: NEML2 -- the New Engineering material Model Library, version 2
// By: Argonne National Laboratory
// OPEN SOURCE LICENSE (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#include <torch/torch.h>
#include "neml2/base/OptionSet.h"

namespace neml2
{
/**
 * @brief The base class of all "manufacturable" objects in the NEML2 library.
 *
 * NEML2 uses the standard registry-factory pattern for automatic object registration and creation.
 * The registry and the factory relies on polymophism to collect and resolve all types at run-time.
 * See `Registry` and `Factory` for more details.
 */
class NEML2Object
{
public:
  static OptionSet expected_options();

  /**
   * @brief Construct a new NEML2Object object
   *
   * @param options The set of options extracted from the input file
   */
  NEML2Object(const OptionSet & options);

  virtual ~NEML2Object() = default;

  const OptionSet & options() const { return _options; }

  /// A readonly reference to the object's name
  const std::string & name() const { return _options.name(); }
  /// A readonly reference to the object's type
  const std::string & type() const { return _options.type(); }
  /// A readonly reference to the object's path
  const std::string & path() const { return _options.path(); }
  /// A readonly reference to the object's docstring
  const std::string & doc() const { return _options.doc(); }

private:
  const OptionSet & _options;
};
} // namespace neml2
