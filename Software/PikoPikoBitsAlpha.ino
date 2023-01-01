//---------------------------
//   PikoPikoBits Alpha
//   YMZ294
//   Sample Program
//   ver 0.1
//   2022/12/31
//   by Synth Senpai
//---------------------------

#include <MIDI.h>
USING_NAMESPACE_MIDI

//*********************************
//Variable

#define NOTE_BD1  35
#define NOTE_BD  36

#define NOTE_SD  38
#define NOTE_SD1  40

#define NOTE_OHH  46
#define NOTE_CHH  42
#define NOTE_HT   41
#define NOTE_MT  45
#define NOTE_LT  48

const int MAXVOICE = 3;
uint8_t note_num[MAXVOICE];

// Output Pins
const byte WRCS_PIN = 13;
const byte CS_PIN = 12;
const byte A0_PIN = 11;
const byte RESET_PIN = 10;
const int DATA_PIN[] = { 9, 8, 7, 6, 5, 4, 3, 2 };
const byte Pod1_Pin = A0;
const byte  Pod2_Pin = A1;
const byte SW1_PIN = A2;
const byte  SW2_PIN = A3;
const byte LED1_PIN = A4;
const byte LED2_PIN = A5;

int pod1_val, pod2_val ;
int sw1_state, sw2_state ;

const word note2f[] = {
  8.2 , 8.7 , 9.2 , 9.7 , 10.3 , 10.9 , 11.6 , 12.2 , 13 , 13.8 , 14.6 , 15.4 , 16.4 , 17.3 , 18.4 , 19.4 , 20.6 , 21.8 , 23.1 , 24.5 , 26 , 27.5 , 29.1 , 30.9 , 32.7 , 34.6 , 36.7 , 38.9 , 41.2 , 43.7 , 46.2 , 49 , 51.9 , 55 , 58.3 , 61.7 , 65.4 , 69.3 , 73.4 , 77.8 ,
  82.4 , 87.3 , 92.5 , 98 , 103.8 , 110 , 116.5 , 123.5 , 130.8 , 138.6 , 146.8 , 155.6 , 164.8 , 174.6 , 185 , 196 ,
  207.7 , 220 , 233.1 , 246.9 , 261.6 , 277.2 , 293.7 , 311.1 , 329.6 , 349.2 , 370 , 392 , 415.3 , 440 , 466.2 ,
  493.9 , 523.3 , 554.4 , 587.3 , 622.3 , 659.3 , 698.5 , 740 , 784 , 830.6 , 880 , 932.3 , 987.8 , 1046.5 , 1108.7 ,
  1174.7 , 1244.5 , 1318.5 , 1396.9 , 1480 , 1568 , 1661.2 , 1760 , 1864.7 , 1975.5 , 2093 , 2217.5 , 2349.3 , 2489 , 2637 ,
  2793.8 , 2960 , 3136 , 3322.4 , 3520 , 3729.3 , 3951.1 , 4186 , 4434.9 , 4698.6 , 4978 , 5274 , 5587.7 , 5919.9 , 6271.9 , 6644.9 ,
  7040 , 7458.6 , 7902.1 , 8372 , 8869.8 , 9397.3 , 9956.1 , 10548.1 , 11175.3 , 11839.8 , 12543.9
};

int vmode = 0 ;
// vmode = 0  noemal 
// vmode=  1  decay envelop

int mode = 0 ;
// mode = 0  ; midi 1-16  without 10
// mode = 1    midi 10  drum tone


int vno = 0 ;   // current voice no for vmode = 1



//midi
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

//*********************************************
// setup

void setup() {

  for (int i = 0; i < 8; i++) {
    pinMode(DATA_PIN[i], OUTPUT);
  }
  pinMode(WRCS_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  pinMode(A0_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  reset();

  set_register(0x07, 0b00111000);
  set_register(0x08, 0b00000111);
  set_register(0x09, 0b00000111);
  set_register(0x0a, 0b00000111);

  if (vmode == 1)
  {
    set_register(0x0C, 8);
    set_register(0x08, 0b00010111);
    set_register(0x09, 0b00010111);
    set_register(0x0A, 0b00010111);
  }

  SetFrequency(0, 0);
  SetFrequency(1, 0);
  SetFrequency(2, 0);

  //midi
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange( handleControlChange );
  MIDI.setHandleProgramChange( handleProgramChange );
  MIDI.setHandlePitchBend( handlePitchBend );
  MIDI.begin(MIDI_CHANNEL_OMNI);



}
// Setup end
//*********************************************

//*********************************************
// Main



void loop() {

  MIDI.read();
  pod1_val = analogRead(Pod1_Pin);
  pod2_val = analogRead(Pod1_Pin);
  sw1_state = digitalRead(SW1_PIN);
  sw2_state = digitalRead(SW2_PIN);
  if (sw1_state == 0)
  {
    midi_panic();
  }

  if (sw2_state == 0)
  {
    change_mode();
  }

  //  set_volume(pod1_val / 64);
}

//  Main end
//*********************************************


//********************************************
//MIDI

void change_mode()
{
  if (mode == 0)
  {
    set_register(0x07, 0b00110110);
    set_register(0x08, 0b00011111);
    mode = 1;
    digitalWrite(LED2_PIN, HIGH);
    midi_panic();
  } else
  {
    set_register(0x07, 0b00111000);
    set_register(0x08, 0b00000111);
    if (vmode == 1)
    {
      set_register(0x0C, 8);
      set_register(0x08, 0b00010111);
      set_register(0x09, 0b00010111);
      set_register(0x0A, 0b00010111);

    }
    mode = 0;
    digitalWrite(LED2_PIN, LOW);

  }



  delay(500);


}






void midi_panic()
{
  for (int k = 0 ; k < MAXVOICE ; k++)
  {
    note_off(k, 0);
    note_num[k] = 0;
  }

}

void set_volume(int vol)
{

  set_register(0x08, vol);
  set_register(0x09, vol);
  set_register(0x0a, vol);

}



void handleNoteOn(byte channel, byte pitch, byte velocity)
{
  int keyno ;
  if (channel != 10 && mode == 0 && vmode == 0 )
  {
    boolean keyflg = false ;
    keyno = pitch;
    keyflg = false ;
    for (int k = 0 ; k < MAXVOICE ; k++)
    {
      if (note_num[k] == 0 and keyflg == false ) {
        keyflg = true;
        note_on(k, keyno);
        note_num[k] = keyno ;
      }
    }
  }

  if (channel != 10 && mode == 0 && vmode == 1 )
  {
    keyno = pitch;
    note_on1(vno, keyno);
    vno++;
    if (vno = MAXVOICE)
    {
      vno = 0;
    }


  }



  if (channel == 10 && mode == 1)
  {
    if (pitch == NOTE_BD || pitch == NOTE_BD1)
    {
      d_bd();
    }
    if (pitch == NOTE_SD || pitch == NOTE_SD1)
    {
      d_sd();
    }
    if (pitch == NOTE_CHH)
    {
      d_chh();
    }
    if (pitch == NOTE_OHH)
    {
      d_ohh();
    }
    if (pitch == NOTE_HT)
    {
      d_ht();
    }
    if (pitch == NOTE_MT)
    {
      d_mt();
    }
    if (pitch == NOTE_LT)
    {
      d_lt();
    }
  }


}






void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  int keyno ;
  if (channel != 10 && vmode == 0)
  {
    keyno = pitch ;
    for (int k = 0 ; k < MAXVOICE ; k++)
    {
      if (note_num[k] == keyno) {
        note_off(k, keyno);
        note_num[k] = 0;
      }
    }
  }

}


void handleControlChange(byte channel , byte number , byte value )
{

}

void handleProgramChange(byte channel , byte number )
{

}

void handlePitchBend(byte channel , int bend )
{

}


void note_on(int voice_no, int key_no)
{
  SetFrequency(voice_no, note2f[key_no]);
  digitalWrite(LED1_PIN, HIGH);
}


void note_on1(int voice_no, int key_no)
{
  SetFrequency(voice_no, note2f[key_no]);
  set_register(0x0d, 0b00000000);
  digitalWrite(LED1_PIN, HIGH);
}




void note_off(int voice_no, int key_no)
{
  SetFrequency(voice_no, 0);
  digitalWrite(LED1_PIN, LOW);
}


//drum
void d_lt()
{
  SetFrequency(0, 160);
  set_register(0x06, 16);
  set_register(0x0C, 10);
  set_register(0x0d, 0b00000000);
}
void d_ht()
{
  SetFrequency(0, 640);
  set_register(0x06, 16);
  set_register(0x0C, 10);
  set_register(0x0d, 0b00000000);
}
void d_mt()
{
  SetFrequency(0, 320);
  set_register(0x06, 16);
  set_register(0x0C, 10);
  set_register(0x0d, 0b00000000);
}
void d_chh()
{
  SetFrequency(0, 0);
  set_register(0x06, 0);
  set_register(0x0C, 2);
  set_register(0x0d, 0b00000000);
}

void d_ohh()
{
  SetFrequency(0, 0);
  set_register(0x06, 0);
  set_register(0x0C, 15);
  set_register(0x0d, 0b00000000);
}
void d_sd()
{
  SetFrequency(0, 0);
  set_register(0x06, 24);
  set_register(0x0C, 8);
  set_register(0x0d, 0b00000000);
}
void d_bd()
{
  SetFrequency(0, 40);
  set_register(0x06, 30);
  set_register(0x0C, 4);
  set_register(0x0d, 0b00000000);
}


// 初期化
void reset() {
  set_register(0x00, 0);
  set_register(0x01, 0);
  set_register(0x02, 0);
  set_register(0x03, 0);
  set_register(0x04, 0);
  set_register(0x05, 0);
  set_register(0x06, 0);

  set_register(0x07, 0);
  set_register(0x08, 0);
  set_register(0x09, 0);
  set_register(0x0a, 0);

  set_register(0x0b, 0);
  set_register(0x0c, 0);
  set_register(0x0d, 0);


  digitalWrite(CS_PIN, HIGH);
  digitalWrite(WRCS_PIN, HIGH);
  digitalWrite(A0_PIN, LOW);
  digitalWrite(RESET_PIN, LOW);
  delay(10);
  digitalWrite(RESET_PIN, HIGH);

}

void SetFrequency(int ch, word freq) {
  word cal_freqency = 0;
  if (freq != 0) {
    cal_freqency = 125000 / freq;
  }
  cal_freqency &= 0b0000111111111111;
  set_register(0x00 + (ch * 2), cal_freqency & 0xff);
  set_register(0x01 + (ch * 2), (cal_freqency >> 8) & 0xff);
}

// レジスタセット
void set_register(byte addr, byte value)
{
  // addr
  digitalWrite(WRCS_PIN, LOW);
  digitalWrite(CS_PIN, LOW);
  digitalWrite(A0_PIN, LOW);
  for (int i = 0; i < 8; i++) {
    digitalWrite(DATA_PIN[i], bitRead(addr, i));
  }
  digitalWrite(WRCS_PIN, HIGH);
  digitalWrite(CS_PIN, HIGH);
  // value
  digitalWrite(WRCS_PIN, LOW);
  digitalWrite(CS_PIN, LOW);
  digitalWrite(A0_PIN, HIGH);
  for (int i = 0; i < 8; i++) {
    digitalWrite(DATA_PIN[i], bitRead(value, i));
  }
  digitalWrite(WRCS_PIN, HIGH);
  digitalWrite(CS_PIN, HIGH);
}
