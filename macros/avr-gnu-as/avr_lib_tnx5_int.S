#.org  0                      // 0
#      RJMP _jp_reset
#.org  INT0_vect_num*2        // 1
#      RJMP
#.org  PCINT0_vect_num*2      // 2
#      RJMP
#.org  TIM1_COMPA_vect_num*2  // 3
#      RJMP
#.org  TIM1_OVF_vect_num*2    // 4
#      RJMP
#.org  TIM0_OVF_vect_num*2    // 5
#      RJMP
#.org  EE_RDY_vect_num*2      // 6
#      RJMP
#.org  ANA_COMP_vect_num*2    // 7
#      RJMP
#.org  ADC_vect_num*2         // 8
#      RJMP
#.org  TIM1_COMPB_vect_num*2  // 9
#      RJMP
#.org  TIM0_COMPA_vect_num*2  // 10
#      RJMP
#.org  TIM0_COMPB_vect_num*2  // 11
#      RJMP
#.org  WDT_vect_num*2         // 12
#      RJMP
#.org  USI_START_vect_num*2   // 13
#      RJMP
#.org  USI_OVF_vect_num*2     // 14
#      RJMP
#.org  _VECTORS_SIZE
