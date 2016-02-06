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

#include "TetrahedronEdgeExprModel.hh"
#include "Region.hh"
#include "Edge.hh"
#include "Vector.hh"
#include "ModelExprEval.hh"
#include "GeometryStream.hh"
#include "dsAssert.hh"
#include "EngineAPI.hh"
#include <set>
#include <string>
#include <sstream>


// Must be valid equation object which is passed
TetrahedronEdgeExprModel::TetrahedronEdgeExprModel(const std::string &nm, const Eqo::EqObjPtr eq, RegionPtr rp, TetrahedronEdgeModel::DisplayType dt) : TetrahedronEdgeModel(nm, rp, dt), equation(eq)
{
#if 0
  os << "creating TetrahedronEdgeExprModel " << nm << " with equation " << eq << "\n";
#endif
  RegisterModels();
}

void TetrahedronEdgeExprModel::RegisterModels()
{
    typedef std::set<std::string> refmodels_t;
    refmodels_t refs;

    if (EngineAPI::getEnumeratedType(equation) == EngineAPI::MODEL_OBJ)
    {
        refs.insert(EngineAPI::getStringValue(equation));
    }
    else if (EngineAPI::getEnumeratedType(equation) == EngineAPI::VARIABLE_OBJ)
    {
        refs.insert(EngineAPI::getStringValue(equation));
    }
    else
    {
        refmodels_t refmodels    = EngineAPI::getReferencedType(equation, EngineAPI::MODEL_OBJ);
        refmodels_t refvariables = EngineAPI::getReferencedType(equation, EngineAPI::VARIABLE_OBJ);

        for (refmodels_t::iterator it = refmodels.begin(); it != refmodels.end(); ++it)
        {
            refs.insert(*it);
        }
        for (refmodels_t::iterator it = refvariables.begin(); it != refvariables.end(); ++it)
        {
            refs.insert(*it);
        }
    }

    for (refmodels_t::iterator it = refs.begin(); it != refs.end(); ++it)
    {
#if 0
    os << "registering callback " << *it << " for TetrahedronEdgeExprModel " << this->GetName() << "\n";
#endif
        RegisterCallback(*it);
    }
}

void TetrahedronEdgeExprModel::calcTetrahedronEdgeScalarValues() const
{
    // need to write the calculator
//    dsAssert(false, "UNEXPECTED");
#if 0
        os << "updating TetrahedronEdgeExprModel " << GetName() << " from expression " << EngineAPI::getStringValue(equation) << "\n";
#endif
    MEE::ModelExprEval::error_t errors;
    const Region *rp = &(this->GetRegion());
    MEE::ModelExprEval mexp(rp, GetName(), errors);
    MEE::ModelExprData out = mexp.eval_function(equation);

    if (!errors.empty())
    {
        std::ostringstream os; 
        os << "while evaluating tetrahedron edge model " << GetName() << 
        " on Device: " << GetRegion().GetDeviceName() <<
        " on Region: " << GetRegion().GetName()
        << "\n";
        for (MEE::ModelExprEval::error_t::iterator it = errors.begin(); it != errors.end(); ++it)
        {
            os << *it << "\n";
        }
        GeometryStream::WriteOut(OutputStream::ERROR, *rp, os.str());
    }

    /// implicit conversion to tetrahedronedgemodel from edgemodel
    if (
        (out.GetType() == MEE::ModelExprData::EDGEDATA)
       )
    {
      out.convertToTetrahedronEdgeData();
    }

    if (
        (out.GetType() == MEE::ModelExprData::TETRAHEDRONEDGEDATA)
       )
    {
      const MEE::ModelExprData::ScalarValuesType &tval = out.GetScalarValues();
      if (tval.IsUniform())
      {
        SetValues(tval.GetScalar());
      }
      else
      {
        TetrahedronEdgeScalarList nsl(tval.GetVector());
        SetValues(nsl);
      }
    }
    else if (out.GetType() == MEE::ModelExprData::DOUBLE)
    {
        const double v = out.GetDoubleValue();
        SetValues(v);
    }
    else
    {
        std::ostringstream os; 
        os << "while evaluating model " << GetName() << ": expression "
            << EngineAPI::getStringValue(equation) << " evaluates to " << MEE::ModelExprData::datatypename[out.GetType()]
            << "\n";
        GeometryStream::WriteOut(OutputStream::FATAL, *rp, os.str());
    }
        
}

void TetrahedronEdgeExprModel::Serialize(std::ostream &of) const
{
  of << "COMMAND element_model -device \"" << GetDeviceName() << "\" -region \"" << GetRegionName() << "\" -name \"" << GetName() << "\" -equation \"" << EngineAPI::getStringValue(equation) << ";\" -display_type \"" << GetDisplayTypeString() << "\"";
}

