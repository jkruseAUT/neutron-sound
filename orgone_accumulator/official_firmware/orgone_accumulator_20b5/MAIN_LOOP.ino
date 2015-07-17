void loop() {

  if (loopReset == 1)goto evilGoto; //these will jump to the same place in the loop when a gateISR happens.

  if (LED_MCD > 0)LED_MCD = LED_MCD - 1;
  if (LED_MCD == 1)WRITE2EEPROM();

  
  //slow generated signals
  noiseTable2[random(0, 512)] = random(-32767, 32767); //hypnotoad noise (noiseTable2)
  
  if (loopReset == 1)goto evilGoto;
  SWC ++;
  fuh = analogControls[0] >> 6;
  if (SWC > fuh) {
    NT3Rate = (random(-7, 8)) - (noiseTable3[0] / 4198); //LF noise (noiseTable3)
    SWC = 0;
  }

  if (loopReset == 1)goto evilGoto;
  ARC ++;
  if ((ARC == 7 || ARC == 9) && tuneLockOn) ARC ++; //skip reading tuning knobs if tunelock is on.
  if (ARC > 9) {
    ARC = 0;
  }  //cycle through analog controls. skip tuning controls if tunelock is on.
  if (loopReset == 1)goto evilGoto;
  //--------------------------------------------------------
  inCV = (analogRead(A0)); //main v/oct CV in. only use 12 bits of analog in SEPERATE AINS BY CODE
  //--------------------------------------------------------
  if (loopReset == 1)goto evilGoto;
  //______________TUNING OPTIONS  
  tuner = inCV + ((analogControls[9] >> 8) * (conf_NoteSize * tuneStep)) + (analogControls[7] >> 4); //coarse and fine tuning
  //comment out above line and uncomment following line for analog style non stepped tuning
  //tuner = inCV+(analogControls[9]>>1)+(analogControls[7]>>4);
  //_____________END TUNINGOPTIONS


  if (loopReset == 1)goto evilGoto;
  inputScaler = float(tuner / octaveSize);
  if (loopReset == 1)goto evilGoto;

  //______________________________CONVERSION OPTIONS
  //digitalWriteFast (oSQout,0);//temp testing OC
  //inputVOct = powf(2.0,inputScaler); //uncomment for slightly more accurate v/oct conversion (but it is slower)
  inputVOct = fastpow2(inputScaler); //"real time" exponentiation of CV input! (powf is single precision float power function) comment out if using powf version above
  //digitalWriteFast (oSQout,1);//temp testing OC
  //______________________________END CONVERSION OPTIONS


  //inputVOct = sq(inputScaler);
  if (loopReset == 1)goto evilGoto;
  inputConverter = inputVOct * conf_TuneMult; //<-----------------------------number is MASTER TUNING also affected by ISR speed
  //divide by 2 if you want it play 1 octave lower, 4 for 2 octaves etc.
  //you can also fine tune it to just how you like it by tweaking the number up and down.

  if (loopReset == 1)goto evilGoto;
  //---------------------------------------------------------------------

  READ_POTS();

  //----------------------------------------------------------------------
  if (loopReset == 1)goto evilGoto;
  //running average filter of ratio CV and index CV they are sensetive to noise.
  totalaInRAv = totalaInRAv - readingsaInRAv[indexaInRAv];
  totalaInIAv = totalaInIAv - readingsaInIAv[indexaInRAv];

  readingsaInRAv[indexaInRAv] = AInRawFilter = (8191 - analogRead(A17)); //adjust numreadingsaInRAv on first page for filtering.
  readingsaInIAv[indexaInRAv] = aInModIndex;

  totalaInRAv = totalaInRAv + readingsaInRAv[indexaInRAv];
  totalaInIAv = totalaInIAv + readingsaInIAv[indexaInRAv];

  indexaInRAv = indexaInRAv + 1;
  if (indexaInRAv >= numreadingsaInRAv) indexaInRAv = 0;

  averageaInRAv = (totalaInRAv / numreadingsaInRAv);
  averageaInIAvCubing = (4095 - (totalaInIAv / numreadingsaInRAv)) / 512.0;
  averageaInIAv = totalaInIAv / numreadingsaInRAv;


evilGoto:


  //------------------------------Position CV---------------------
  aInPos = 4095 - ((analogRead(A16))) ;
  //------------------------------------------------------------------
  loopReset = 0;
  envVal = constrain((aInPos + mixPos), 0, 4095); //mix the position knob with the modulation from the CV input (fix for bipolar)

  mixMid = constrain((2047 - abs(envVal - 2047)), 0, 2047); //sets the level of the midpoint wave
  mixHi = constrain((((envVal)) - 2047), 0, 2047); //sets the level of the high wave
  mixLo = constrain((2047 - ((envVal))), 0, 2047); //sets the level of the low wave



  if (loopReset == 1)goto evilGoto;
  //---------------------------Detune CV----------------
  aInDetuneReading = analogRead(A12);
  aInModDetune = ((4095 - aInDetuneReading) << 1);
  //--------------------------------------------------

  DODETUNING();

  if (loopReset == 1)goto evilGoto;
  //---------------------------Index CV---------------------------------
  aInModIndex = analogRead(A15);
  //----------------------------------------------------------------------


  if (FX == 3) ASSIGNINCREMENTS_P();//pulsar
  else if (FX == 7 || FX == 8) ASSIGNINCREMENTS_C();//chord
  else if (FX == 2  ) ASSIGNINCREMENTS_D();//delay
  else if (FX == 9  ) ASSIGNINCREMENTS_A();//ADT
  else if (FX == 11  ) ASSIGNINCREMENTS_DRUM();//drum voice
  else ASSIGNINCREMENTS();//default



  if (loopReset == 1)goto evilGoto;
  UPDATE_LEDS();
  
  UPDATE_prog_LEDS();

  if (IsHW2) {

    if (FXCycleButton.update()) {
      if (FXCycleButton.fallingEdge()) {
        FXi = FXi + 1;      
        if (FXi > FX_Count) FXi = 0;       
        FX = FXa[FXi];
        SELECT_ISRS();
        
      }
    }


    if (FXCycleButtonDn.update()) {
      if (FXCycleButtonDn.fallingEdge()) {
        FXi = FXi - 1;
        if (FXi < 0) FXi = FX_Count;        
        FX = FXa[FXi];
        SELECT_ISRS();
        
      }
    }


  }

  else {
    if (FXCycleButton.update()) {
      if (tuneLockOn) {
        if (FXCycleButton.fallingEdge()) {
          FXi = FXi - 1;
          if (FXi < 0) FXi = FX_Count;
          FX = FXa[FXi];
          SELECT_ISRS();
        }
        if (FXSw == 1 && FXCycleButton.risingEdge()) {
          FXi = FXi - 1;
          if (FXi < 0) FXi = FX_Count;
          FX = FXa[FXi];
          SELECT_ISRS();
        }
      }


      else {
        if (FXCycleButton.fallingEdge()) {
          FXi = FXi + 1;
          if (FXi > FX_Count) FXi = 0;
          FX = FXa[FXi];
          SELECT_ISRS();
        }
        if (FXSw == 1 && FXCycleButton.risingEdge()) {
          FXi = FXi + 1;
          if (FXi > FX_Count) FXi = 0;
          FX = FXa[FXi];
          SELECT_ISRS();
        }
      }
    }
  }
  
  // ----------------------------------
  monitorSerialReception();
  if(requestForData) {
    requestForData = false;
    sendBroadcastPacket();
  }
    
//  if(monitorSerialReception) 
//  if(BROADCAST != 0) {  
//    static int pace = 0;
//    if (++pace > 20) {
//      pace = 0;
//      sendBroadcastPacket();
//    }
//  }
  // ----------------------------------



}






