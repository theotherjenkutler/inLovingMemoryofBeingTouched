#include <AccelStepper.h>

#include <MIDI.h>
float notes[] = {
  8.1757989156,   // 0
  8.6619572180,   // 1
  9.1770239974,   // 2
  10.3008611535,  // 3
  10.3008611535,  // 4
  10.9133822323,  // 5
  11.5623257097,  // 6
  12.2498573744,  // 7
  12.9782717994,  // 8
  13.7500000000,  // 9
  14.5676175474,  // 10
  15.4338531643,  // 11
  16.3515978313,  // 12
  17.3239144361,  // 13
  18.3540479948,  // 14
  19.4454364826,  // 15
  20.6017223071,  // 16
  21.8267644646,  // 17
  23.1246514195,  // 18
  24.4997147489,  // 19
  25.9565435987,  // 20
  27.5000000000,  // 21
  29.1352350949,  // 22
  30.8677063285,  // 23
  32.7031956626,  // 24
  34.6478288721,  // 25
  36.7080959897,  // 26
  38.8908729653,  // 27
  41.2034446141,  // 28
  43.6535289291,  // 29
  46.2493028390,  // 30
  48.9994294977,  // 31
  51.9130871975,  // 32
  55.0000000000,  // 33
  58.2704701898,  // 34
  61.7354126570,  // 35
  65.4063913251,  // 36
  69.2956577442,  // 37
  73.4161919794,  // 38
  77.7817459305,  // 39
  82.4068892282,  // 40
  87.3070578583,  // 41
  92.4986056779,  // 42
  97.9988589954,  // 43
  103.8261743950,  // 44
  110.0000000000,  // 45
  116.5409403795,  // 46
  123.4708253140,  // 47
  130.8127826503,  // 48
  138.5913154884,  // 49
  146.8323839587,  // 50
  155.5634918610,  // 51
  164.8137784564,  // 52
  174.6141157165,  // 53
  184.9972113558,  // 54
  195.9977179909,  // 55
  207.6523487900,  // 56
  220.0000000000,  // 57
  233.0818807590,  // 58
  246.9416506281,  // 59
  261.6255653006,  // 60
  277.1826309769,  // 61
  293.6647679174,  // 62
  311.1269837221,  // 63
  329.6275569129,  // 64
  349.2282314330,  // 65
  369.9944227116,  // 66
  391.9954359817,  // 67
  415.3046975799,  // 68
  440.0000000000,  // 69
  466.1637615181,  // 70
  493.8833012561,  // 71
  523.2511306012,  // 72
  554.3652619537,  // 73
  587.3295358348,  // 74
  622.2539674442,  // 75
  659.2551138257,  // 76
  698.4564628660,  // 77
  739.9888454233,  // 78
  783.9908719635,  // 79
  830.6093951599,  // 80
  880.0000000000,  // 81
  932.3275230362,  // 82
  987.7666025122,  // 83
  1046.5022612024, // 84
  1108.7305239075, // 85
  1174.6590716696, // 86
  1244.5079348883, // 87
  1318.5102276515, // 88
  1396.9129257320, // 89
  1479.9776908465, // 90
  1567.9817439270, // 91
  1661.2187903198, // 92
  1760.0000000000, // 93
  1864.6550460724, // 94
  1975.5332050245, // 95
  2093.0045224048, // 96
  2217.4610478150, // 97
  2349.3181433393, // 98
  2489.0158697766, // 99
  2637.0204553030, // 100
  2793.8258514640, // 101
  2959.9553816931, // 102
  3135.9634878540, // 103
  3322.4375806396, // 104
  3520.0000000000, // 105
  3729.3100921447, // 106
  3951.0664100490, // 107
  4186.0090448096, // 108
  4434.9220956300, // 109
  4698.6362866785, // 110
  4978.0317395533, // 111
  5274.0409106059, // 112
  5587.6517029281, // 113
  5919.9107633862, // 114
  5919.9107633862, // 115
  6644.8751612791, // 116
  7040.0000000000, // 117
  7458.6201842894, // 118
  7902.1328200980, // 119
  8372.0180896192, // 120
  8869.8441912599, // 121
  9397.2725733570, // 122
  9956.0634791066, // 123
  10548.0818212118, // 124
  11175.3034058561, // 125
  11839.8215267723, // 126
  12543.8539514160 // 127
};

//! Our little onboard LED
int ledPin = 2;

//! The stepper motor controller's step pin
int stpPin = 10;

//! Are we currently playing a note
bool playing = false;

//! The desired delay between motor steps to achieve the requested note/frequency
float desired_delay = 0.0;

//! The current delay between motor steps
float curr_delay = 0.0;

//! The last note that was recived - used to decide when to turn off the motor
byte last_note = 0;

//! The speed at which the controller will glide from one note to the next
/*! Units is the number of microseconds to add to curr_delay per iteration
    until it reaches desired_delay */
int const glide = 0;

//! Convert a midi note to the number of microseconds we need to delay
float note2microseconds(byte note)
{
  return 1.0 / (notes[note] / 100.0);
}


void OnNoteOn(byte channel, byte note, byte velocity)
{
  last_note = note;

  // Set the desired delay by converting the MIDI note
  curr_delay = note2microseconds(note);

  playing = true;
  digitalWrite(ledPin, HIGH);
}

//! Called when we recieve a MIDI note-off event
void OnNoteOff(byte channel, byte note, byte velocity)
{
  // Only turn off the last requested note
  // This makes it more comfortable to jam on the keyboard
  if(note == last_note)
  {
    playing = false;
    digitalWrite(ledPin, LOW);
  }
}

void setup() {                
  pinMode(ledPin, OUTPUT);     

  pinMode(stpPin, OUTPUT);     
  digitalWrite(stpPin, LOW);
}


void loop() {
  while(MIDIUSB.available() > 0){
    Serial.println("MIDI Available");
    MIDIEvent e;
    e = MIDIUSB.read();
    //Note On Shows up in the Serial Monitor as e.type "8"
    if(e.type == 8){
      OnNoteOff(0, e.m2, e.m3);
      Serial.println("NoteOff");
    } 
    //Note Off Shows up in the Serial Monitor as e.type "9"
    else if(e.type == 9){
       OnNoteOn(0, e.m2, e.m3);
       Serial.println("NoteOn");
    }
  }  

  if(playing) {
    digitalWrite(stpPin, HIGH);
    Serial.println("StepperPinHigh");
    Serial.println(curr_delay);
    delayMicroseconds(5);
    digitalWrite(stpPin, LOW);
    delayMicroseconds(curr_delay);
  }
}

