void gateISR() {
if (digitalReadFast(gateIn)){
  
  gateState = 1;

  declickRampOut = 4095;
  declickValue = AGCtest;

  o1.phase =
    o2.phase =
      o3.phase =
        o4.phase =
          o5.phase =
            o6.phase =
              o7.phase =
                o8.phase =
                  o9.phase =
                    lfo.phase =
                      o10.phase = 0;
  //detuneScaler = 0;
  //cycleCounter= 0;
  //ARC = SWC = 0;


  drum_envStep[0] =
    drum_envStep[1] =
    drum_envStep[2] =
      drum_st =
        drum_st2 = 0;

  loopReset = 1;  
  //analogWrite(aout2,4000);
  //aInPos = 4095-((analogRead(A16))) ;
  //    interrupts();
}

else {
   gateState = 0;
}
}
  