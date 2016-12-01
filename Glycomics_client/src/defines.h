//
//  defines.h
//  Glycomics
//
//  Created by Jonas Fehr on 14/11/16.
//
//

#define RENDER_W 1440
#define RENDER_H 3440

#define FONT_SIZE 60
#define FONT_SIZE_TITLE 76
#define WORDSPACING 30 // deprechiated
#define SPACES 0
#define BORDER_L FONT_SIZE_TITLE*1.5
#define BORDER_R 50

#define HAS_POINTS true
#define POINTS_SIZE 5

#define BEZIER_LENGTH 150

#define ALPHA_STEP .01


// TRANSFORM

#define PROB_TRANSFORM 0.001


// WIND SIMULATION

#define NOISE_SPACING_DIV 500
#define NOISE_TIMER_ALL_DIV 20
#define NOISE_TIMER_FINE_DIV NOISE_TIMER_ALL_DIV/2

#define MIX_NOISE 0.8
#define NOISE_FORCE_MUL 4
#define SPRING_ORIGIN_STRENGTH	0.001


// OSC
#define RECEIVE_PORT 3335

#define SEND_PORT 3334
#define SEND_HOST "localhost"

#define NUM_MSG_STRINGS 20



// PHYSICS SYSTEM

#define DRAG 0.99

#define UNITS(s)            (s * ofGetWidth() / 1280.0) // scale to uniform units

#define SPRING_STRENGTH         0.1

#define	SPRING_WIDTH	UNITS(1)

#define RADIUS			UNITS(5)

#define MASS				1

#define BOUNCE				0.2

#define FORCE_AMOUNT		UNITS(10)

#define EDGE_DRAG			0.98

#define	GRAVITY				0

#define RETRACTION			-1

#define ATTRACTION          0.1

#define SECTOR_COUNT		1		// currently there is a bug at sector borders, so setting this to 1
