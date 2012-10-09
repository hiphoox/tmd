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
const char* PROGRAM_NAME = "tmd";

void usage(void)
{
  fprintf(stderr, "usage: %s    #toggle main display\n"
          "       %s -m #toggle mirror option\n", PROGRAM_NAME, PROGRAM_NAME);
  exit(EXIT_FAILURE);
}

void setMainDisplay(CGDisplayCount maxDisplays, CGDirectDisplayID targetDisplay)
{
  CGDisplayErr       dErr;
  CGDisplayCount     numberOfOnlineDisplays, displayIndex;
  CGDirectDisplayID  onlineDisplays[MAX_DISPLAYS];
	CGDisplayConfigRef configRef;
  int				         deltaX, deltaY;
  
  dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &numberOfOnlineDisplays);
  if (dErr != kCGErrorSuccess) {
    fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
    exit(1);
  }
	  
	deltaX = -CGRectGetMinX (CGDisplayBounds (targetDisplay));
  deltaY = -CGRectGetMinY (CGDisplayBounds (targetDisplay));
  
  CGError err = CGBeginDisplayConfiguration (&configRef);
  if (err != kCGErrorSuccess) {
      fprintf(stderr, "Error with CGBeginDisplayConfiguration: %d\n",err);
      exit(EXIT_FAILURE);
  }
  
  for (displayIndex = 0; displayIndex < numberOfOnlineDisplays; displayIndex++) {
    CGDirectDisplayID dID = onlineDisplays[displayIndex];
    
    CGConfigureDisplayOrigin (configRef, dID,
                              CGRectGetMinX (CGDisplayBounds (dID)) + deltaX,
                              CGRectGetMinY (CGDisplayBounds (dID)) + deltaY );
	}
  
  err = CGCompleteDisplayConfiguration (configRef, kCGConfigureForSession);
  if (err != kCGErrorSuccess) {
    fprintf(stderr, "Error with CGCompleteDisplayConfiguration: %d\n",err);
    exit(EXIT_FAILURE);
  }
}

void toggleMirror(CGDisplayCount maxDisplays, CGDirectDisplayID secondaryDisplay)
{
  CGDisplayCount    numberOfActiveDisplays;
  CGDirectDisplayID activeDisplays[] = {0,0};
  CGDisplayConfigRef configRef;
  
  CGDisplayErr activeError = CGGetActiveDisplayList (maxDisplays, activeDisplays, &numberOfActiveDisplays);
  if (activeError != kCGErrorSuccess) {
      fprintf(stderr, "Error in obtaining active diplay list: %d\n",activeError);
      exit(EXIT_FAILURE);
  }
  
  CGError err = CGBeginDisplayConfiguration (&configRef);
  if (err != kCGErrorSuccess) {
      fprintf(stderr, "Error with CGBeginDisplayConfiguration: %d\n",err);
      exit(EXIT_FAILURE);
  }
  
  if (numberOfActiveDisplays == MAX_DISPLAYS) { // Displays are unmirrored -> mirror them
      err = CGConfigureDisplayMirrorOfDisplay (configRef, secondaryDisplay, CGMainDisplayID());
  } else { // Displays are mirrored -> unmirror them
      err = CGConfigureDisplayMirrorOfDisplay (configRef, secondaryDisplay, kCGNullDirectDisplay);
  }

  if (err != kCGErrorSuccess) {
    fprintf(stderr, "Error with the switch commands!: %d\n",err);
    exit(EXIT_FAILURE);
  }

  err = CGCompleteDisplayConfiguration (configRef,kCGConfigurePermanently);
  if (err != kCGErrorSuccess) {
    fprintf(stderr, "Error with CGCompleteDisplayConfiguration: %d\n",err);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char * argv[])
{
  CGDisplayErr      dErr;
  CGDisplayCount    displayCount;
  CGDirectDisplayID secondaryDisplay;
  CGDisplayCount    maxDisplays = MAX_DISPLAYS;
  CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
  int opt, mflag;

  while ((opt = getopt(argc, argv, "m")) != -1)
  {
    switch (opt)
    {
      case 'm':
        mflag = 1;
        break;
      default:
        usage();
        break;
    }
  }

  dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
  if (dErr != kCGErrorSuccess) {
    fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
    exit(EXIT_FAILURE);
  }
  
  if (displayCount != MAX_DISPLAYS) {
    fprintf(stderr, "We just support toggle between two displays \n");
    exit(EXIT_FAILURE);
  }
  
  if (onlineDisplays[FIRST_DISPLAY_INDEX] == CGMainDisplayID()){
    secondaryDisplay = onlineDisplays[SECOND_DISPLAY_INDEX];
  } else {
    secondaryDisplay = onlineDisplays[FIRST_DISPLAY_INDEX];
  }
  
  if (mflag == TRUE) {
    toggleMirror(maxDisplays, secondaryDisplay);
  } else {
    setMainDisplay(maxDisplays, secondaryDisplay);
  }
  
  return 0;
}

