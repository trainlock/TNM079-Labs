/*************************************************************************************************
 *
 * Modeling and animation (TNM079) 2007
 * Code base for lab assignments. Copyright:
 *   Gunnar Johansson (gunnar.johansson@itn.liu.se)
 *   Ken Museth (ken.museth@itn.liu.se)
 *   Michael Bang Nielsen (bang@daimi.au.dk)
 *   Ola Nilsson (ola.nilsson@itn.liu.se)
 *   Andreas Sˆderstrˆm (andreas.soderstrom@itn.liu.se)
 *
 *************************************************************************************************/
#ifndef __operatoradvect_h__
#define __operatoradvect_h__

#include "Levelset/LevelSetOperator.h"
#include "Math/Function3D.h"
#include "Math/Matrix4x4.h"

/*! \brief A level set operator that does external advection
 *
 * This class implements level set advectionr in an external vector field by the
 * PDE
 *
 *  \f$
 *  \dfrac{\partial \phi}{\partial t} + \mathbf{V}(\mathbf{x})\cdot \nabla \phi
 * = 0 \f$
 */
//! \lab4 Implement advection in external vector field
class OperatorAdvect : public LevelSetOperator {
protected:
  Function3D<Vector3<float>> *mVectorField;

public:
  OperatorAdvect(LevelSet *LS, Function3D<Vector3<float>> *vf)
      : LevelSetOperator(LS), mVectorField(vf) {}

  virtual float ComputeTimestep() {
    // Compute and return a stable timestep
    // (Hint: Function3D::GetMaxValue())
	float dx = mLS->GetDx();
	Vector3<float> V = mVectorField->GetMaxValue();

	float dt = std::min(std::min(dx / V[0], dx / V[1]), dx / V[2]);

    return dt;
  }

  virtual void Propagate(float time) {
    // Determine timestep for stability
    float dt = ComputeTimestep();

    // Propagate level set with stable timestep dt
    // until requested time is reached
    for (float elapsed = 0; elapsed < time;) {

      if (dt > time - elapsed)
        dt = time - elapsed;
      elapsed += dt;

      IntegrateEuler(dt);
      // IntegrateRungeKutta(dt);
    }
  }

  virtual float Evaluate(size_t i, size_t j, size_t k) {
	// Compute the rate of change (dphi/dt)

	// Remember that the point (i,j,k) is given in grid coordinates, while
	// the velocity field used for advection needs to be sampled in
	// world coordinates (x,y,z). You can use LevelSet::TransformGridToWorld()
	// for this task.
	float x = (float)i, y = (float)j, z = (float)k;
	mLS->TransformGridToWorld(x, y, z);

	Vector3<float> V = mVectorField->GetValue(x, y, z);

	float ddx, ddy, ddz;
	if (V[0] < 0) {
		ddx = mLS->DiffXp(i, j, k);
	}
	else if (V[0] > 0) {
		ddx = mLS->DiffXm(i, j, k);
	}
	else {
		ddx = 0;
	}

	if (V[1] < 0) {
		ddy = mLS->DiffYp(i, j, k);
	}
	else if (V[1] > 0) {
		ddy = mLS->DiffYm(i, j, k);
	}
	else {
		ddy = 0;
	}

	if (V[2] < 0) {
		ddz = mLS->DiffZp(i, j, k);
	}
	else if (V[2] > 0) {
		ddz = mLS->DiffZm(i, j, k);
	}
	else {
		ddz = 0;
	}

	Vector3<float> gradient = { ddx, ddy, ddz };

	return (gradient * (-V));
  }
};

#endif
