//
//  defines.h
//  Glycomics
//
//  Created by Jonas Fehr on 14/11/16.
//
//

#define RENDER_W 1080
#define RENDER_H 2560

#define FONT_SIZE 32
#define FONT_SIZE_TITLE 42
#define WORDSPACING 20
#define BORDER_L FONT_SIZE_TITLE*1.5
#define BORDER_R 50

// OSC
#define RECEIVE_PORT 3335

#define SEND_PORT 3334
#define SEND_HOST "localhost"

#define NUM_MSG_STRINGS 20



// PHYSICS SYSTEM

#define UNITS(s)            (s * ofGetWidth() / 1280.0) // scale to uniform units

#define SPRING_STRENGTH		0.1

#define	SPRING_WIDTH	UNITS(1)

#define RADIUS			UNITS(5)

#define MASS				1

#define BOUNCE				0.2

#define FORCE_AMOUNT		UNITS(10)

#define EDGE_DRAG			0.98

#define	GRAVITY				-0.5

#define RETRACTION			-1

#define ATTRACTION          1

#define SECTOR_COUNT		1		// currently there is a bug at sector borders, so setting this to 1
