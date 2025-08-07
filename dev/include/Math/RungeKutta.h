#pragma once

#include "Math/Vector3D.h"

#include <functional>


/** @brief ルンゲ・クッタ法 */
inline void RungeKutta( Vector3Dd& f, double& t, double dt, Vector3Dd& df, std::function<void( double, Vector3Dd, Vector3Dd& )> eqn )
{
	Vector3Dd df1, df2, df3;
	Vector3Dd g;

	const double DT2( dt * 0.5 );
	const double DT6( dt / 6.0 );

	eqn( t, f, df1 );
	g = f + DT2 * df1;

	eqn( t + DT2, g, df2 );
	g = f + DT2 * df2;

	eqn( t + DT2, g, df3 );
	g = f + dt * df3;

	eqn( t + dt, g, df );
	f += DT6 * ( df1 + 2.0 * ( df2 + df3 ) + df );

	//eqn( t + DT2, g, df3 );
	//g = f + dt * df3;
	//df3 += df2;

	//eqn( t + dt, g, df2 );
	//f += DT6 * ( df1 + df2 + ( 2.0 * df3 ) );

	t += dt;
}
