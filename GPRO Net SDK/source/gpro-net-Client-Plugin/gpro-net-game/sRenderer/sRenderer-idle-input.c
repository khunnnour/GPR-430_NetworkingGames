/*
	Copyright 2011-2021 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	sRenderer_idle-input.c/.cpp
	Demo state function implementations.

	****************************************************
	*** THIS IS ONE OF YOUR DEMO'S MAIN SOURCE FILES ***
	*** Implement your demo logic pertaining to      ***
	***     HANDLING INPUT in this file.             ***
	****************************************************
*/

//-----------------------------------------------------------------------------

#include "sRenderer.h"
#include "_utility/sRenderer-macros.h"


//-----------------------------------------------------------------------------
// INPUT SUB-ROUTINES

void renderer_input_controlProjector_xcontrol(
	a3_XboxControllerInput const* xcontrol,
	sProjectorComponent const* projector, a3f64 const dt)
{
	a3real azimuth = a3real_zero;
	a3real elevation = a3real_zero;
	a3real zoom = a3real_zero;

	a3f64 const moveRate = dt * projector->dataPtr->ctrlMoveSpeed;
	a3f64 const rotateRate = dt * projector->dataPtr->ctrlRotateSpeed, ctrlRotateRate = rotateRate * 15.0;
	a3f64 const zoomRate = dt * projector->dataPtr->ctrlZoomSpeed, zoomLimit = (180.0 - (a3f64)zoomRate);

	// move and rotate camera using joysticks
	a3f64 lJoystick[2], rJoystick[2], lTrigger, rTrigger;
	a3XboxControlGetJoysticks(xcontrol, lJoystick, rJoystick);
	a3XboxControlGetTriggers(xcontrol, &lTrigger, &rTrigger);

	renderer_moveProjectorSceneObject(projector,
		(a3real)(moveRate * (+lJoystick[0])),
		(a3real)(moveRate * (rTrigger - lTrigger)),
		(a3real)(moveRate * (-lJoystick[1]))
	);
	// rotate
	{
		azimuth = (a3real)(-rJoystick[0]);
		elevation = (a3real)(+rJoystick[1]);

		// this really defines which way is "up"
		// mouse's Y motion controls pitch, but X can control yaw or roll
		// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
		renderer_rotateProjectorSceneObject(projector,
			// pitch: vertical tilt
			(a3real)ctrlRotateRate * elevation,
			// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
			a3real_zero,
			(a3real)ctrlRotateRate * azimuth);
	}

	// zoom
	projector->dataPtr->fovy -= (a3real)zoomRate
		* (a3real)((a3XboxControlGetState(xcontrol, a3xbox_leftBumper) / a3xbox_leftBumper
			- a3XboxControlGetState(xcontrol, a3xbox_rightBumper) / a3xbox_rightBumper));

	// clamp zoom
	projector->dataPtr->fovy = a3clamp((a3real)zoomRate, (a3real)zoomLimit, projector->dataPtr->fovy);
	renderer_updateProjector(projector);
}

void renderer_input_controlProjector_trad(
	a3_MouseInput const* mouse, a3_KeyboardInput const* keyboard,
	sProjectorComponent const* projector, a3f64 const dt)
{
	a3real azimuth = a3real_zero;
	a3real elevation = a3real_zero;
	a3real zoom = a3real_zero;

	a3f64 const moveRate = dt * projector->dataPtr->ctrlMoveSpeed;
	a3f64 const rotateRate = dt * projector->dataPtr->ctrlRotateSpeed, ctrlRotateRate = rotateRate * 15.0;
	a3f64 const zoomRate = dt * projector->dataPtr->ctrlZoomSpeed, zoomLimit = (180.0 - (a3f64)zoomRate);

	// move using WASDEQ
	renderer_moveProjectorSceneObject(projector,
		(a3real)(moveRate * (a3f64)a3keyboardGetDifference(keyboard, a3key_D, a3key_A)),
		(a3real)(moveRate * (a3f64)a3keyboardGetDifference(keyboard, a3key_E, a3key_Q)),
		(a3real)(moveRate * (a3f64)a3keyboardGetDifference(keyboard, a3key_S, a3key_W))
	);
	if (a3mouseIsHeld(mouse, a3mouse_left))
	{
		azimuth = -(a3real)a3mouseGetDeltaX(mouse);
		elevation = -(a3real)a3mouseGetDeltaY(mouse);

		// this really defines which way is "up"
		// mouse's Y motion controls pitch, but X can control yaw or roll
		// controlling yaw makes Y axis seem "up", roll makes Z seem "up"
		renderer_rotateProjectorSceneObject(projector,
			// pitch: vertical tilt
			(a3real)rotateRate * elevation,
			// yaw/roll depends on "vertical" axis: if y, yaw; if z, roll
			a3real_zero,
			(a3real)rotateRate * azimuth);
	}

	// zoom
	projector->dataPtr->fovy -= (a3real)zoomRate
		* (a3real)(a3mouseIsChanged(mouse, a3mouse_wheel));

	// clamp zoom
	projector->dataPtr->fovy = a3clamp((a3real)zoomRate, (a3real)zoomLimit, projector->dataPtr->fovy);
	renderer_updateProjector(projector);
}


//-----------------------------------------------------------------------------
// CALLBACKS

// ascii key callback
void renderer_input_keyCharPress(sRenderer* renderer, a3i32 const asciiKey)
{
	switch (asciiKey)
	{
		// toggle grid
		rendererCtrlCaseToggle(renderer->displayGrid, 'g');

		// toggle skybox
		rendererCtrlCaseToggle(renderer->displaySkybox, 'b');

		// toggle world axes
		rendererCtrlCaseToggle(renderer->displayWorldAxes, 'x');

		// toggle object axes
		rendererCtrlCaseToggle(renderer->displayObjectAxes, 'z');
	}
}

// ascii key hold callback
void renderer_input_keyCharHold(sRenderer* renderer, a3i32 asciiKey)
{

}


//-----------------------------------------------------------------------------
