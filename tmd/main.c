//
//  main.c
//  tmd
//
//  Created by Norberto Ortigoza on 10/8/12.
//  Copyright (c) 2012 Raku. All rights reserved.
//

#include <ApplicationServices/ApplicationServices.h>

const int MAX_DISPLAYS = 2;
const int FIRST_DISPLAY_INDEX = 0;
const int SECOND_DISPLAY_INDEX = 1;

void setMainDisplay(CGDirectDisplayID targetDisplay)
{
  CGDisplayErr       dErr;
  CGDisplayCount     displayCount, displayIndex;
  CGDisplayCount     maxDisplays = MAX_DISPLAYS;
  CGDirectDisplayID  onlineDisplays[MAX_DISPLAYS];
	CGDisplayConfigRef config;
  int				         deltaX, deltaY;
  
  dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
  if (dErr != kCGErrorSuccess) {
    fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
    exit(1);
  }
	  
	deltaX = -CGRectGetMinX (CGDisplayBounds (targetDisplay));
  deltaY = -CGRectGetMinY (CGDisplayBounds (targetDisplay));
  
  CGBeginDisplayConfiguration (&config);
  
  for (displayIndex = 0; displayIndex < displayCount; displayIndex++) {
    CGDirectDisplayID dID = onlineDisplays[displayIndex];
    
    CGConfigureDisplayOrigin (config, dID,
                              CGRectGetMinX (CGDisplayBounds (dID)) + deltaX,
                              CGRectGetMinY (CGDisplayBounds (dID)) + deltaY );
	}
  
  CGCompleteDisplayConfiguration (config, kCGConfigureForSession);    
}

int main(int argc, const char * argv[])
{
  CGDisplayErr      dErr;
  CGDisplayCount    displayCount;
  CGDirectDisplayID mainDisplay;
  CGDisplayCount    maxDisplays = MAX_DISPLAYS;
  CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
  int targetDisplayIndex = FIRST_DISPLAY_INDEX;
  
  mainDisplay = CGMainDisplayID();
  
  dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
  if (dErr != kCGErrorSuccess) {
    fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
    exit(1);
  }
  
  if (displayCount != 2) {
    fprintf(stderr, "We just support toggle between two displays \n");
    exit(1);
  }
  
  if (mainDisplay == onlineDisplays[FIRST_DISPLAY_INDEX]){
    targetDisplayIndex = SECOND_DISPLAY_INDEX;
  }
  
  setMainDisplay(onlineDisplays[targetDisplayIndex]);

  return 0;
}

