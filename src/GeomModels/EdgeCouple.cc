/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "EdgeCouple.hh"
#include "Region.hh"
#include "Edge.hh"
#include "dsAssert.hh"
#include "TriangleEdgeModel.hh"
#include "TetrahedronEdgeModel.hh"

EdgeCouple::EdgeCouple(RegionPtr rp) :
EdgeModel("EdgeCouple", rp, EdgeModel::SCALAR)
{
  const size_t dimension = rp->GetDimension();
  if ((dimension == 2) || (dimension == 3))
  {
    RegisterCallback("ElementEdgeCouple");
  }
}


void EdgeCouple::calcEdgeScalarValues() const
{
  const size_t dimension=GetRegion().GetDimension();

  if (dimension == 1)
  {
    std::vector<double> ev(GetRegion().GetNumberEdges(), 1.0);
    SetValues(ev);
  }
  else if (dimension == 2)
  {
    calcEdgeCouple2d();
  }
  else if (dimension == 3)
  {
    calcEdgeCouple3d();
  }
  else
  {
    dsAssert(false, "UNEXPECTED");
  }
}

void EdgeCouple::calcEdgeCouple2d() const
{
  ConstTriangleEdgeModelPtr eec = GetRegion().GetTriangleEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");

  std::vector<double> ev = eec->GetValuesOnEdges();
  SetValues(ev);
}

void EdgeCouple::calcEdgeCouple3d() const
{
  ConstTetrahedronEdgeModelPtr eec = GetRegion().GetTetrahedronEdgeModel("ElementEdgeCouple");
  dsAssert(eec.get(), "ElementEdgeCouple missing");
  std::vector<double> ev = eec->GetValuesOnEdges();
  SetValues(ev);

}

void EdgeCouple::Serialize(std::ostream &of) const
{
  SerializeBuiltIn(of);
}

