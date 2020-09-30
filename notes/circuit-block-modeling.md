
## clock, counter, divider, wave form generator
* all this discussion assume digital square wave, no analog (can discuss in AD/DA)
* clock, source of driver
  - wave `_|¯¦_|¯¦_|¯¦_|¯¦_`
  - frequency => T => T_high or T_low = T/2
  - set freq ()
* Counter, clock drive, generate event when clock numbers reach counter num
  - input clock src
  - input pin
  - output type
    - pulse, toggle 
  - output pin
  - output idle status [high, low]
  - config counter
  - set input
    - pin
  - set output
    - pin
    - type
    - idle status
  - set config
    - counter
  - get config
    - config
    - status [init/start/stop/pause/counter]
  - action
    - init
    - start
    - pause
    - stop
* clock divider
  - prescaler, special counter
    - 1/N, N=2^n n[1,2,3,4...n]
  - counter when any n>0 natural number
    - 1/n, n=2,3,4...n
* wave form [square wave] generator
  - frequency
  - duty cycle = high/low
    - 50% = counter or prescaler
  - other property from counter
  - set frequency like clock
  
```mermaid
graph LR
    clock --> Counter --> WaveFormGen --> Event
    Counter --> Event
```
```mermaid
classDiagram
    class clock{
      clk_Freq
      clk_setFreq()
    }
    class Counter{
      cnt_clk
      cnt_IdleStatus
      cnt_OutputType[pulse,toggle]
      cnt_Counter
      cnt_SetIdleStatus()
      cnt_SetOutputType()
      cnt_SetCounter()
      cnt_GetCounterStatus()
      cnt_GetCounterConfig()
      cnt_Start()
      cnt_Pause()
      cnt_Stop()
    }
    class WaveFormGen{
      wf_Freqency
      wf_WaveType
      wf_OutputPin
      wf_IdleStatus

      wf_SetFreq()
      wf_SetWaveType()
      wf_SetOutputPin()
      wf_SetIdleStatus()
      wf_GetStatus()

      wf_Start()
      wf_Stop()
      wf_Pause()
    }
```
## Modulation
* basic concept
  - information can send directly
    - set/clr pin status, pin status indicate information
  - information can send via carrier
    - send via serialization
    - info to control carrier each modulation unit
    - carrier have high frequency
    - infor need a serializer, bit shifer

- Modulation Example, OOK, On-Off
  ```mermaid
  graph TD
        BitShifter -- Bit --> Modulator[\Modulator<br>AND BIT,1/]
        CC[Carrier Clock] --> Modulator
        Modulator --> OutputSignal
  ```
```mermaid
classDiagram
    class BitShifter{
      sh_Buffer
      sh_BitOut
      sh_BitOutPin
      sh_ShifterStatus

      sh_LoadBuffer()
      sh_SetBitOutPin()
      sh_ShiftOneBit()
      sh_GetShifterStatus()
    }
    class Modulator{
      md_InputLogicSignal
      md_InputPinSingnal
      md_InputLogicCarrier
      md_InputPinCarrier
      md_OutputLogicLevel
      md_OutputPin
      md_SetPin()
      md_DoMod()
      md_GetOutStatus()
    }
```
