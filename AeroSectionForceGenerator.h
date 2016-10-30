#pragma once

class AeroForceGenerator;
class AeroControlSurface;
class AeroSectionElementForceGenerator;

#include "CompositeListForceGenerator.h"
#include "AeroControlSurface.h"
#include "AeroControlSurfaceBoundary.h"
#include "OpenGLPipeline.h"

class AeroSectionForceGenerator : 
	public CompositeListForceGenerator<AeroSectionForceGenerator,AeroSectionElementForceGenerator, AeroForceGenerator>,
	public AeroSectionBoundary<AeroSectionForceGenerator>
{
public:
	FORCEGENERATOR_TYPE( Type_AeroSection )
	AeroSectionForceGenerator(AeroForceGenerator *p);

    void drawForceGenerator(GSRigidBody *b, Renderer *r) override;
	void setRecorderHook(FlightRecorder& a) override;

	void addControlSurface(AeroControlSurface *pControlSurface);
	AeroControlSurface* controlSurfaceN(int index);
	const AeroControlSurface* controlSurfaceN(int index) const;
	size_t controlSurfaceCount();
	double getEfficiency();

	class AeroPivotObject : public PivotObject
	{
	public:
		AeroPivotObject(AeroControlSurface* pControlSurface, int index=-1, PivotObject *pParent=nullptr) :
			PivotObject(pParent), 
			_pControlSurface( pControlSurface ), 
			_deflectionFactor(1.0),
			_index(index)
		{}

		enum AeroPivotType
		{
			AeroPivotType_None,
			AeroPivotType_Offset,
			AeroPivotType_Linear,
			AeroPivotType_Trig,
		};

		struct AeroPivotRTAdjustorComponent
		{
			AeroPivotType pivotType;
			float value;
		};

		struct AeroPivotRTAdjustor
		{
			AeroPivotRTAdjustorComponent T[3];
		} _RTAdjustors;

		void setPivot(Vector3F t, Vector3F r, std::vector<std::pair<std::string,std::string>>& RTAdjustors, float fDeflectionFactor=1.0f)
		{
			PivotObject::setPivot(t, r);
			_deflectionFactor = fDeflectionFactor;
			
			static std::map<std::string,AeroPivotType> strPivotAdjustMap;
				
			if( !strPivotAdjustMap.size() )
			{
				strPivotAdjustMap["nomod"] = AeroPivotType::AeroPivotType_None;
				strPivotAdjustMap["offset"] = AeroPivotType::AeroPivotType_Offset;
				strPivotAdjustMap["linear"] = AeroPivotType::AeroPivotType_Linear;
				strPivotAdjustMap["trig"] = AeroPivotType::AeroPivotType_Trig;
			}

			for( size_t i = 0; i < RTAdjustors.size(); ++i )
			{
				_RTAdjustors.T[i].pivotType = strPivotAdjustMap[RTAdjustors[i].first];
				_RTAdjustors.T[i].value = (float) atof( RTAdjustors[i].second.c_str() );
			}
		}

		void applyTransform(unsigned int camID) override
		{
            GSForceGenerator::PivotObject* po = getParent();
			if (po)
				po->applyTransform(camID);

			OpenGLMatrixStack& mv = OpenGLPipeline::Get(camID).GetModel();

			Vector3F T = getT();

			for( int i=0; i < 3; ++i )
			{
				if( AeroPivotType::AeroPivotType_Offset == _RTAdjustors.T[i].pivotType )
				{
					*(&T.x + i) += _RTAdjustors.T[i].value;
				}
				else if( AeroPivotType::AeroPivotType_Linear == _RTAdjustors.T[i].pivotType )
				{
					*(&T.x + i) += _RTAdjustors.T[i].value * _pControlSurface->getDeflection();
				}
			}
	
			mv.Top().Translate(T);
			mv.Top().RotateView(getR());
	
			if( _pControlSurface )
			{
				if( !_index )
					mv.Top().Translate(-_pControlSurface->getShiftTranslation());

				mv.Top().RotateModel(-_pControlSurface->getDeflection() * _deflectionFactor, 0, 0);
			}

			mv.Top().RotateModel(getR());
			mv.Top().Translate(-getT());
		}

		float getDeflection()
		{
			if( _pControlSurface == 0 )
				return 0.0;

			return _pControlSurface->getDeflection();
		}

	private:
		AeroControlSurface* _pControlSurface;
		int _index;
		float _shiftTranslation;
		float _deflectionFactor;
	};

private:
	double _efficiency;
    std::vector<AeroControlSurface*> _controlSurfaces;
};

