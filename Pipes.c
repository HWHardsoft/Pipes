/*
 *  Pipe dream clone for Uzebox 
 *  Version 1.0
 *  Copyright (C) 2012  Hartmut Wendt
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


//#define _Fading	// enables FadeIn & FadeOut between the different screens
#define _Music	// enables Music
#define Music_volume 50	// volume for music
#define SFX_volume 250	// volume for sound effects

#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "kernel/uzebox.h"
#include "data/pipes_BGTiles.pic.inc"
#include "data/fonts.pic.inc"
#include "data/animations.inc"
#include "data/patches.inc"
#ifdef _Music
 #include "data/pipes_tracks.inc"
#endif



/* global definitons */
// program modes
enum {
	PM_Intro,		// program mode intro
	PM_Game_Start,	// program mode: Game starts
	PM_Game_Flow,	// program mode: Game water flow
	PM_Game_End,	// program mode: Game ends
	PM_Credits,		// shows credits
	PM_HoF_view,	// view hall of fame
	PM_HoF_edit,	// edit hall of fame
	PM_Game_over,	// program mode for game over screen
	PM_Game_Start_Menu,	// small menu during game start
	PM_Game_Flow_Menu,	// small menu during game 
	PM_Help_Screen1,// help screen 1
	PM_Help_Screen2,// help screen 2
	PM_Help_Screen3,// help screen 3
	PM_Help_Screen4,// help screen 4
};


//pipes
#define max_pipes	81
enum {
	Empty,
	Start_left_empty,
	Start_right_empty,
	Start_up_empty,
	Start_down_empty,
	Start_left_full,
	Start_right_full,
	Start_up_full,
	Start_down_full,
	End_left_empty,
	End_right_empty,
	End_up_empty,
	End_down_empty,
	End_left_full,
	End_right_full,
	End_up_full,
	End_down_full,
	Pump_ver_empty,
	Pump_hor_empty,
	Pump_ver_full,
	Pump_hor_full,
	Reservoir_ver_empty,
	Reservoir_hor_empty,
	Reservoir_ver_full,
	Reservoir_hor_full,
	Straight_ver_empty,
	Straight_hor_empty,
	Straight_ver_full,
	Straight_hor_full,
	Edge_upper_left_empty,
	Edge_upper_left_full,
	Edge_lower_left_empty,
	Edge_lower_left_full,
	Edge_upper_right_empty,
	Edge_upper_right_full,
	Edge_lower_right_empty,
	Edge_lower_right_full,
	Cross_hor_empty_ver_empty,
	Cross_hor_full_ver_empty,
	Cross_hor_empty_ver_full,
	Cross_hor_full_ver_full,
	Arrow_right_empty,
	Arrow_right_full,
	Arrow_left_empty,
	Arrow_left_full,
	Arrow_up_empty,
	Arrow_up_full,
	Arrow_down_empty,
	Arrow_down_full,
	Cursor_straight_ver,
	Cursor_straight_hor,
	Cursor_edge_upper_left,
	Cursor_edge_lower_left,
	Cursor_edge_upper_right,
	Cursor_edge_lower_right,
	Cursor_cross,
	Cursor_arrow_right,
	Cursor_arrow_left,
	Cursor_arrow_up,
	Cursor_arrow_down
};



// difficulty
enum {
	Easy,		// difficulty easy
	Medium,		// difficulty medium
	Hard,		// difficulty hard
};	


// flow directions
enum {
	flow_left,
	flow_right,
	flow_up,
	flow_down
};	



struct pipe_stack_options {
	u8 CursorMap;
	u8 StackMap;
	u8 GameMap;
};

	

// general parameters

// 8-bit, 255 period LFSR (for generating pseudo-random numbers)
#define PRNG_NEXT() (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))))
#define Flow_Speed_Easy	28
#define Flow_Speed_Medium 24
#define Flow_Speed_Hard	20




//#define delta_t 1 

#define MAX(x,y) ((x)>(y) ? (x) : (y))


struct EepromBlockStruct ebs;



struct pipe_stack_options pipe_stack[5];


u8 prng; // Pseudo-random number generator
u8 program_mode;	// program mode (intro, 1 player mode, 2 player mode, ....
u8 PosX=0, PosY=0;	 // position of cursor
//u8 current_step;
int iSCORE;			// Highscore
bool Music_on= true;
u8 Start_point_X;
u8 Start_point_Y;
u8 End_point_X,End_point_Y;
u8 ani_cur = 0;
u8 Flow_direction;
u8 Flow_ani_cnt = 0;
u8 Flow_speed;
u8 Flow_step = 0;
int Flow_animation[4];
u8 Difficulty = Easy;
u8 Tunnel_ver;
u8 Tunnel_hor;
u8 Wrenches=3;
u8 Distance;
u8 pump_cnt;
u8 loop_cnt;
u8 menu_background[40];
u8 CUR_POS_GM; 

static u8 playing_field[max_pipes];

/*** function prototypes *****************************************************************/
void init(void);
void set_PM_mode(u8 mode);
void copy_buf(unsigned char *BUFA, unsigned char *BUFB, unsigned char ucANZ);
void fill_buf(unsigned char *BUFA, unsigned char value, unsigned char ucANZ);
void msg_window(u8 x1, u8 y1, u8 x2, u8 y2, bool background, bool shadow);
int get_highscore(u8 entry);
u8 check_highscore(void);
void copy_highsore(u8 entry_from, u8 entry_to);
void clear_highsore(u8 entry);
u8 set_def_highscore(void);
u8 view_highscore_entry(u8 x, u8 y, u8 entry, u8 load_data, u8 uDiff);
void edit_highscore_entry(u8 entry, u8 cursor_pos, u8 b_mode);
void show_highscore_char(u8 entry, u8 position, u8 cursor_on);
int GetTile(u8 xx, u8 yy);
void create_new_playing_field(u8 uDiff);
u8 check_pipe_area(u8 uX, u8 uY);
void init_pipe_stack(u8 uDiff);
void show_pipe_stack(void);
void cycle_pipe_stack(u8 uDiff);
void pipe_stack_random_map(u8 uDiff, u8 index);
u8 get_pipe(u8 uX, u8 uY);
void set_pipe_cursor(u8 xx, u8 yy);
void hide_pipe_cursor(u8 xx, u8 yy);
void place_pipe(u8 uX, u8 uY, u8 uPipe);
void set_pipe(u8 uX, u8 uY, u8 uPipe);
u8 next_pipe(bool bStart);
void Draw_Wrench_reserve(void);
void Draw_Intro_char(u8 uX, u8 uY, const int *PChar, u8 columns);
void animate_pipes(void);
void animate_start(u8 uDiff);
void animate_explosion(u8 *uX,u8 *uY, u8 *Step, bool BEndGame);
void end_game(void);
u8 read_eep_block(u8 uDiff);
void dbg_highscore_print(u8 entry, u8 position);
//void fade_out_volume(void);
void set_flow_speed(void);
void save_background(u8 x1, u8 y1,u8 x2, u8 y2);
void restore_background(u8 x1, u8 y1,u8 x2, u8 y2); 



void init(void)
// init program
{

  //Set the tile table to use.
  SetTileTable(BGTiles);

  // init font table
  SetFontTable(fonts);
	

  // init music	
  InitMusicPlayer(patches);
  #ifdef _Music
	 StartSong(Pipes_menu_song);
  #endif
  SetMasterVolume(Music_volume);

   // init random generator
  prng = 15;
  
  	
  // load into screen
  set_PM_mode(PM_Intro);

 

  //Use block 25  
  ebs.id = 25;
  if (!isEepromFormatted())
     return;

  if (EEPROM_ERROR_BLOCK_NOT_FOUND == EepromReadBlock(25,&ebs))
  {
	set_def_highscore();
  }		  
  
  //Use block 26  
  ebs.id = 26;
  if (!isEepromFormatted())
     return;

  if (EEPROM_ERROR_BLOCK_NOT_FOUND == EepromReadBlock(26,&ebs))
  {
	set_def_highscore();
  }		



}



int main(){
int ibuttons=0,ibuttons_old;
u8 u1,b=0,uX,uY,uPos_EXP_X,uPos_EXP_Y;
u8 EXP_ANI_CNT = 0;

  // init program
  init();        
  // proceed game	
  while(1)
  {
    WaitVsync(2);	  
    // get controller state
    ibuttons_old = ibuttons;
	ibuttons = ReadJoypad(0);
    switch(program_mode)
	{
	  // proceed intro mode	
	  case PM_Intro:

     	// button A
		if ((BTN_A & ibuttons) && !(ibuttons_old & BTN_A)) {
		  // wait for release of button A 
		  while (BTN_A & ReadJoypad(0)) {
		    prng++; 		
			WaitVsync(1);
		  }
      	  prng = MAX(prng,1);
		

		  if (PosY == 0) set_PM_mode(PM_Game_Start);	
		  else if (PosY == 1) {
	    	if (Difficulty == Hard) {
		  		Print(13,18,PSTR("  EASY"));
				Difficulty = Easy;
	    	} else if (Difficulty == Easy) {
		  		Print(13,18,PSTR("MEDIUM"));
				Difficulty = Medium;
	    	} else {
		  		Print(13,18,PSTR("  HARD"));
				Difficulty = Hard;
			}
		  }
		  
		  else if (PosY == 2) set_PM_mode(PM_Credits);	
  		  else if (PosY == 3) {
		    iSCORE = 0; 
		    set_PM_mode(PM_HoF_view);
		  }				
		  else if (PosY == 4) {
		    if (Music_on) {
			  SetMasterVolume(0); 	
			  Music_on = false;			  
			  Print(10,21,PSTR("MUSIC OFF"));
			  StopSong();
		    } else {
			  SetMasterVolume(Music_volume);
			  Music_on = true;			  
			  Print(10,21,PSTR(" MUSIC ON"));
			  ResumeSong();
			}
		  } else set_PM_mode(PM_Help_Screen1);
		  
 		}
		// UP button
		if ((ibuttons & BTN_UP) && !(ibuttons_old & BTN_UP)) {

		  // wait for release of button BTN_UP 
		  while (BTN_UP & ReadJoypad(0));

		  PrintChar(20,17+PosY,' '); 	
		  if (PosY) PosY--;	
		  PrintChar(20,17+PosY,'$'); 	
		  

		// DOWN button
		} else if ((ibuttons & BTN_DOWN) && !(ibuttons_old & BTN_DOWN)) {

		  // wait for release of button BTN_DOWN 
		  while (BTN_DOWN & ReadJoypad(0));

		  PrintChar(20,17+PosY,' '); 	
		  if (PosY<5) PosY++;	
		  PrintChar(20,17+PosY,'$'); 	
		  
		}

        break;

	  // game play	
	  case PM_Game_Start:
      case PM_Game_Flow:

	    uX = PosX;
	    uY = PosY;
		// check controller action
     	if ((BTN_LEFT & ibuttons) && !(BTN_LEFT & ibuttons_old) && (uX)) uX--;
     	else if ((BTN_RIGHT & ibuttons) && !(BTN_RIGHT & ibuttons_old) && (uX<8)) uX++;
     	if ((BTN_UP & ibuttons) && !(BTN_UP & ibuttons_old) && (uY)) uY--;
     	else if ((BTN_DOWN & ibuttons) && !(BTN_DOWN & ibuttons_old) && (uY<8)) uY++;	
		
		set_pipe_cursor(uX,uY);		

     	if ((BTN_A & ibuttons) && !(ibuttons_old & BTN_A)) {
			// place pipe
			u1 = get_pipe(PosX,PosY);
			if (u1 == Empty) {
				place_pipe(PosX,PosY,pipe_stack[0].GameMap);
				playing_field[(PosY * 9) + PosX] = pipe_stack[0].GameMap;
				cycle_pipe_stack(Difficulty);
				show_pipe_stack();
				TriggerFx(4, 0x7f, true);				
			} else if (((u1 == Straight_ver_empty) || (u1 == Straight_hor_empty) || (u1 == Edge_upper_left_empty) ||
					   (u1 == Edge_lower_right_empty) || (u1 == Edge_lower_left_empty) || (u1 == Edge_upper_right_empty) ||
					   (u1 == Cross_hor_empty_ver_empty) || (u1 == Arrow_right_empty) || (u1 == Arrow_left_empty) ||
					   (u1 == Arrow_up_empty) || (u1 == Arrow_down_empty)) && !(EXP_ANI_CNT)) {
				//place_pipe(PosX,PosY,*pipe_maps[Empty]);
				playing_field[(PosY * 9) + PosX] = pipe_stack[0].GameMap;
				cycle_pipe_stack(Difficulty);
				show_pipe_stack();				
				if (iSCORE >= 5) iSCORE -= 5;

				// set values for pipe explosion
				uPos_EXP_X = PosX;
				uPos_EXP_Y = PosY;
				EXP_ANI_CNT = 11;
				TriggerFx(5, 0x7f, true);
            }					

		}

     	// button START
		else if ((BTN_START & ibuttons) && !(ibuttons_old & BTN_START)) {
		    if (program_mode == PM_Game_Start) set_PM_mode(PM_Game_Start_Menu);		
			else set_PM_mode(PM_Game_Flow_Menu);			  
	
		}

		// show score & Distance
		PrintInt(20,2,iSCORE,false);
		PrintByte(16,4,Distance,false); 


		// handle animation explosion
		animate_explosion(&uPos_EXP_X,&uPos_EXP_Y,&EXP_ANI_CNT,false);


		// handle animation pipe flow
		if (program_mode == PM_Game_Flow) {			
			Flow_step++;
			if (Flow_step >= Flow_speed) {
				Flow_step = 0;
				animate_pipes();

			}	

		} else if (program_mode == PM_Game_Start) {			
			Flow_step++;
			if (Flow_step >= Flow_speed) {
				Flow_step = 0;
				animate_start(Difficulty);

			}	
		}

        break;


		// game play ends	
	    case PM_Game_End:

    	if ((ibuttons) && (ibuttons != ibuttons_old)) {
		  // wait for release of all buttons 
		  while (ReadJoypad(0)) {
		    prng++; 		
			WaitVsync(1);
		  }
      	  prng = MAX(prng,1);
		  if (Wrenches) set_PM_mode(PM_Game_Start);
		  else {
		     StopSong;
		     set_PM_mode(PM_HoF_view);
 			 #ifdef _Music
	 			 if (Music_on == true) StartSong(Pipes_menu_song);
  			 #endif
		  }
		}

		break;

	  // menu in games
	  case PM_Game_Start_Menu:
	  case PM_Game_Flow_Menu:
	  	// UP button
		if ((ibuttons & BTN_UP) && !(ibuttons_old & BTN_UP)) {
		  PrintChar(17,13+CUR_POS_GM,' '); 	
		  if (CUR_POS_GM) CUR_POS_GM--;	
		  PrintChar(17,13+CUR_POS_GM,'$'); 	

		// DOWN button
		} else if ((ibuttons & BTN_DOWN) && !(ibuttons_old & BTN_DOWN)) {
		  PrintChar(17,13+CUR_POS_GM,' '); 	
		  if (CUR_POS_GM<1) CUR_POS_GM++;	
		  PrintChar(17,13+CUR_POS_GM,'$');	
		}
		if ((BTN_A & ibuttons) && !(ibuttons_old & BTN_A)) {
		  // wait for release of button A 
		  while (BTN_A & ReadJoypad(0));
		  ibuttons = 0;
		  if (CUR_POS_GM) {			
 			#ifdef _Music
	 			if (Music_on == true) StartSong(Pipes_menu_song);
  			#endif
			set_PM_mode(PM_Intro);
		  } else {
		    if (program_mode == PM_Game_Start_Menu) program_mode = PM_Game_Start;		
			else program_mode = PM_Game_Flow;	
			restore_background(12,12,19,16);
						
		  }
		}
	    break;


	  // show credits screen...
	  case PM_Credits:			
     	if ((ibuttons) && (ibuttons != ibuttons_old)) {
		  // wait for release of all buttons 
		  while (ReadJoypad(0));
		  set_PM_mode(PM_Intro);		
		}		
        break;


	  case PM_HoF_view:				
		// time control				
		if (b <= 200) b++;
		if (b == 50) Print(9,22,PSTR("PRESS ANY KEY"));	
		if (((ibuttons & BTN_A) && !(ibuttons_old & BTN_A) && (b > 50)) || (b > 200))
		{
		  b = 0;	
		  set_PM_mode(PM_Intro);
		}
		break;

	  case PM_HoF_edit:				
		// cursor blinking
		b++;
		if (b >= 10) b = 0;
		// proceed cursor position with left & right button
		if ((ibuttons & BTN_RIGHT) && !(ibuttons_old & BTN_RIGHT)) {
		  if (PosX < 2) {       	   
		  	show_highscore_char(PosY - 1, PosX, 0); 		 
		    PosX++; 			
          }
		}
		if ((ibuttons & BTN_LEFT) && !(ibuttons_old & BTN_LEFT)) {		 
 		  if (PosX) {
		    show_highscore_char(PosY - 1, PosX, 0); 
		    PosX--; 
          }
		}
		// chose character up & down button
		if ((ibuttons & BTN_UP) && !(ibuttons_old & BTN_UP)) {
		  edit_highscore_entry(PosY,PosX,BTN_UP); 
		}
		else if ((ibuttons & BTN_DOWN) && !(ibuttons_old & BTN_DOWN)) {		 
 		  edit_highscore_entry(PosY,PosX,BTN_DOWN); 
		}     
		// show cursor
		show_highscore_char(PosY - 1, PosX, b > 4);

		// store new entry
		if (ibuttons & BTN_A)   
		{
		  // store new highscore 
		  if (Difficulty == Hard) ebs.id = 26;
		  else ebs.id = 25;
		  EepromWriteBlock(&ebs);
		  set_PM_mode(PM_Intro);
		}
		break;

	  case PM_Help_Screen1:
	  case PM_Help_Screen2:
	  case PM_Help_Screen3:
	  case PM_Help_Screen4:
		// Help screens
     	if ((ibuttons) && (ibuttons != ibuttons_old)) {
		  // wait for release of all buttons 
		  while (ReadJoypad(0));
		  program_mode++;
		  if (program_mode > PM_Help_Screen4) set_PM_mode(PM_Intro);
		  else set_PM_mode(program_mode);		
		}			
	  	break;
	}

  }
  
} 



void set_PM_mode(u8 mode) {
// set parameters, tiles, background etc for choosed program mode	
u8 a,b, dmyhighscore[30];
	switch (mode)
	{

	  case	PM_Intro:

		// init variables
		PosY = 0;
		iSCORE = 0;
		Wrenches=3;
   		 	    
		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,2);
		Fill(1,1,28,12,1);

		// draw P
		Draw_Intro_char(2,2,&pipe_logo_P[0],3);
		Draw_Intro_char(9,2,&pipe_logo_I[0],1);
		Draw_Intro_char(12,2,&pipe_logo_P[0],3);
		Draw_Intro_char(19,2,&pipe_logo_E[0],2);
		Draw_Intro_char(24,2,&pipe_logo_S[0],2);


		// menu
		msg_window(8,15,22,24,true,true);

		Print(15,17,PSTR("PLAY $")); 
		// Difficulty
	    if (Difficulty == Easy) {
		  Print(15,18,PSTR("EASY"));
	    } else if (Difficulty == Medium) {
		  Print(13,18,PSTR("MEDIUM"));
	    } else {
		  Print(15,18,PSTR("HARD"));
		}
		        
        Print(12,19,PSTR("CREDITS")); 
        Print(10,20,PSTR("HIGHSCORE")); 
		// Music on/off
	    if (Music_on) {
		  Print(11,21,PSTR("MUSIC ON "));
	    } else {
		  Print(10,21,PSTR("MUSIC OFF "));
		}
		Print(15,22,PSTR("HELP")); 
		

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;
	  

     case	PM_Game_Start:
	 	// game play

		// stop music
		StopSong();
		// full volume
        SetMasterVolume(SFX_volume);	

		// set variables		
		PosX=0, PosY=0;		
		loop_cnt = 0;
		pump_cnt = 0;
		Flow_ani_cnt = 0;

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,2);
		
		// pipe reservoir
		msg_window(1,1,4,11,true,true);
		SetTile(1,1,frame_vertical);
		SetTile(4,1,frame_vertical);		

		// message box
		msg_window(7,1,26,5,true,true);

		Print(9,2,PSTR("SCORE"));
		Print(9,3,PSTR("WRENCHES "));
		Print(9,4,PSTR("DIST"));

		if (Difficulty == Easy) {
		    Print(21,4,PSTR("EASY"));
			Distance = 15;	
		} else if (Difficulty == Medium) {
			Print(19,4,PSTR("MEDIUM"));
			Distance = 20;	
		} else {
			Print(21,4,PSTR("HARD"));
			Distance = 25;	
		}
		set_flow_speed();
		PrintByte(16,4,Distance,false); 
		Draw_Wrench_reserve();

		// playing field
		msg_window(7,6,26,25,false,true);
		create_new_playing_field(Difficulty);		

		for(a=0; a<9 ; a++)
		for(b=0; b<9 ; b++) place_pipe(a,b,playing_field[(b * 9) + a]);
		

		// pipe stack
		init_pipe_stack(Difficulty);
		show_pipe_stack();

		// draw progressbar
		for(a=0; a<20 ; a++) SetTile(27,a+6,time_bar);
		

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

 		#ifdef _Music
	 		 if (Music_on == true) StartSong(Pipes_game_song);
  		#endif

		break;
	

      case	PM_Credits:
	 	// show credits

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,2);
		msg_window(4,4,26,23,true,true);

        Print(7,6,PSTR("PIPES FOR UZEBOX"));  
		Print(9,7,PSTR("VERSION 1.0"));  

        Print(7,10,PSTR("COPYRIGHT % 2012"));   
		Print(7,11,PSTR("BY HARTMUT WENDT"));  
		Print(7,12,PSTR("WWW.HWHARDSOFT.DE"));  


        Print(9,15,PSTR("SOUND TRACK"));   
		Print(7,16,PSTR("BY CARSTEN KUNZ"));  

        Print(8,19,PSTR("LICENSED UNDER"));
        Print(10,20,PSTR("GNU GPL V3"));

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;


      case	PM_Help_Screen1:
	 	// help screen 1
		// introduce all the crazy pipes

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,1);
		

        Print(8,1,PSTR("HELP - PAGE 1/4"));  


		// first row
		DrawMap(4,4,pipe_straight_ve);
		DrawMap(14,4,pipe_edge_ul_e);
		DrawMap(24,4,pipe_cross_ve_he);		 

		Print(1,7,PSTR("STRAIGHT"));  
        Print(12,7,PSTR("CORNER"));   
		Print(23,7,PSTR("CROSS"));  
		
		// second row
		DrawMap(4,12,pipe_arrow_left_he);
		DrawMap(14,12,start_piece_de);
		SetTile(14,12,Start_sign);
		DrawMap(24,12,start_piece_de);		 
		SetTile(24,12,End_sign);

		Print(1,15,PSTR("ONE-WAY"));  
        Print(12,15,PSTR("START"));   
		Print(23,15,PSTR("END"));  

		// third row
		DrawMap(4,20,reservoir_ve);
		DrawMap(14,20,pump_he);
		SetTile(22,20,frame_horizontal);
		SetTile(23,20,frame_horizontal);
		SetTile(24,20,GB_Black);
		SetTile(25,20,GB_Black);
		SetTile(26,20,frame_horizontal);
		SetTile(27,20,frame_horizontal);

		Print(1,23,PSTR("RESERVOIR"));  
        Print(13,23,PSTR("PUMP"));   
		Print(22,23,PSTR("TUNNEL"));  

        Print(8,26,PSTR("PRESS ANY KEY"));

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;


      case	PM_Help_Screen2:
	 	// help screen 2
		// pipes basics

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,1);
		

        Print(8,1,PSTR("HELP - PAGE 2/4"));  


		Print(1, 3,PSTR("THE OBJECT IS TO SCORE AS"));  
		Print(1, 4,PSTR("MANY POINTS AS POSSIBLE BY"));  
		Print(1, 5,PSTR("CONSTRUCTING A CONTINUOUS"));  
		Print(1, 6,PSTR("PIPE FROM THE STARTING PIECE"));  

		Print(1, 8,PSTR("THE NEXT PIECE TO BE PLAYED"));  
		Print(1, 9,PSTR("APPEARS ON THE BOTTOM OF THE"));  
		Print(1,10,PSTR("DISPENSER (LEFT) AND OVER"));  
		Print(1,11,PSTR("THE PLAYFIELD."));  

		Print(1,13,PSTR("YOU MUST PLAY THE PIPES IN"));  
		Print(1,14,PSTR("THE ORDER THAT THEY COME."));  
		Print(1,15,PSTR("YOU CAN PLACE A PIPE ANY-"));  
		Print(1,16,PSTR("WHERE YOU LIKE WITH THE"));  
		Print(1,17,PSTR("BUTTON A."));  

		Print(1,19,PSTR("YOU CAN 'BLAST' A PREVIOUSLY"));  
		Print(1,20,PSTR("PLAYED PIECE BY PLACING A"));  
		Print(1,21,PSTR("NEW PIECE ON TOP OF IT."));  
		Print(1,22,PSTR("SPECIAL PIPE PIECES (GREY)"));  
		Print(1,23,PSTR("MAY NOT BE 'BLASTED'."));  

        Print(8,26,PSTR("PRESS ANY KEY"));

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;


      case	PM_Help_Screen3:
	 	// help screen 3
		// pipes additional information

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,1);
		

        Print(8,1,PSTR("HELP - PAGE 3/4"));  


		Print(1, 3,PSTR("THE DIST(ANCE) COUNTER IN"));  
		Print(1, 4,PSTR("THE UPPER RIGHT OF THE"));  
		Print(1, 5,PSTR("SCREEN SHOWS THE REMAINING"));  
		Print(1, 6,PSTR("NUMBER OF PIPE PIECES TO BE"));  
		Print(1, 7,PSTR("FILLED TO REACH MINIMUM"));  
		Print(1, 8,PSTR("DISTANCE."));  

		Print(1,10,PSTR("IF YOU DO NOT REACH MINIMUM"));  
		Print(1,11,PSTR("DISTANCE, YOU WILL LOSE A"));  
		Print(1,12,PSTR("WRENCH. YOU WILL HAVE A"));  
		Print(1,13,PSTR("TOTAL OF THREE WRENCHES."));  
		Print(1,14,PSTR("WHEN THEY ARE GONE, THE GAME"));  
		Print(1,15,PSTR("GAME IS OVER."));  
		
		Print(1,17,PSTR("USE CROSS PIECES WISELY."));  
		Print(1,18,PSTR("EACH ONE CAN DELIVER EXTRA"));  
		Print(1,19,PSTR("POINTS. PUMPS WILL SPEED UP"));
		Print(1,20,PSTR("THE FLOOZ FOR A FEW PIPE"));
		Print(1,21,PSTR("SECTIONS. RESERVOIRS BRIEFLY"));  
		Print(1,22,PSTR("SLOW DOWN THE FLOOZ. TUNNELS"));  
		Print(1,23,PSTR("ALLOW YOU TO CONNECT YOUR."));  
		Print(1,24,PSTR("PIPES TO THE OPPOSITE SIDE."));  

        Print(8,26,PSTR("PRESS ANY KEY"));

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;


      case	PM_Help_Screen4:
	 	// help screen 4
		// scoring

		//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,1);
		

        Print(8,1,PSTR("HELP - PAGE 4/4"));  


		Print(1, 3,PSTR("YOU WILL RECEIVE POINTS FOR"));  
		Print(1, 4,PSTR("THE FOLLOWING WHEN THE FLOOZ"));  
		Print(1, 5,PSTR("FLOW THROUGH THE PIPES:"));	

		Print(5, 7,PSTR("TYPE           SCORE"));  
		Print(5, 8,PSTR("--------------------"));  
		Print(5, 9,PSTR("NORMAL PIPE        5"));  
		Print(5,10,PSTR("ONE-WAY PIPE      10"));  
		Print(5,11,PSTR("RESERVOIR         20"));  
		Print(5,12,PSTR("PUMP             100"));  
		Print(5,13,PSTR("TUNNEL            80"));  
		Print(5,14,PSTR("CROSS              5"));  
		Print(5,15,PSTR("CROSS LOOP        20"));  
		Print(5,16,PSTR("END PIECE         *2"));  

		Print(1,18,PSTR("YOU WILL LOSE 5 POINTS FOR"));  
		Print(1,19,PSTR("EACH PIPE YOU 'BLAST'."));  		
		Print(1,20,PSTR("AT THE END OF EACH ROUND,"));  
		Print(1,21,PSTR("YOU WILL LOSE 10 POINTS FOR"));  
		Print(1,22,PSTR("EACH PIPE ON THE PLAYFIELD"));  
		Print(1,23,PSTR("THAT THE FLOOZ DID NOT PASS"));  
		Print(1,24,PSTR("THROUGH."));  

        Print(8,26,PSTR("PRESS ANY KEY"));

		#ifdef _Fading
		 FadeIn(1, true);
		#endif

		break;

	  case PM_Game_Start_Menu:
	  case PM_Game_Flow_Menu:
	    // save background under menu window
		save_background(12,12,19,16);
		msg_window(12,12,18,15,true,true);
		Print(13,13,PSTR("BACK"));
		Print(13,14,PSTR("MENU"));
  	    // cursor
		PrintChar(17,13,'$');
		CUR_POS_GM = 0;
	    break;



      case	PM_HoF_view:
	    // show the hall of fame

    	//clear screen with fade
		#ifdef _Fading
		 FadeOut(1, true);
		#endif

		ClearVram();
		Fill(0,0,30,28,2);
		msg_window(4,2,26,24,true,true);
		
		// check and sort highscore for actual difficulty
	    PosY = check_highscore();
	    if (PosY == 2) copy_highsore(1,2);
	    if (PosY == 1) {
		  copy_highsore(1,2);
		  copy_highsore(0,1);
        }
		clear_highsore(PosY - 1);

		copy_buf(&ebs.data[0],&dmyhighscore[0],30);

		// reset cursor to left position
		PosX = 0;


		// print headline
		Print(9,4,PSTR("HALL OF FAME"));

		Print(6,7,PSTR("LEVEL   SCORE  NAME"));


		if (Difficulty == Easy) {
            // Easy
	       	view_highscore_entry(6,9,1,!(PosY), Easy);
        	view_highscore_entry(6,10,2,!(PosY), Easy);
        	view_highscore_entry(6,11,3,!(PosY), Easy);  
			// Medium
        	view_highscore_entry(6,13,1,1, Medium);
        	view_highscore_entry(6,14,2,1, Medium);
        	view_highscore_entry(6,15,3,1, Medium);  
			// hard
        	view_highscore_entry(6,17,1,1, Hard);
        	view_highscore_entry(6,18,2,1, Hard);
        	view_highscore_entry(6,19,3,1, Hard);  

        } else if (Difficulty == Medium) {
			// Medium
        	view_highscore_entry(6,13,1,!(PosY), Medium);
        	view_highscore_entry(6,14,2,!(PosY), Medium);
        	view_highscore_entry(6,15,3,!(PosY), Medium);  

        	// EASY
			view_highscore_entry(6,9,1,1, Easy);
        	view_highscore_entry(6,10,2,1, Easy);
        	view_highscore_entry(6,11,3,1, Easy);  

			// Hard
			view_highscore_entry(6,17,1,1, Hard);
        	view_highscore_entry(6,18,2,1, Hard);
        	view_highscore_entry(6,19,3,1, Hard);  
		} else {
			// Hard		
        	view_highscore_entry(6,17,1,!(PosY), Hard);
        	view_highscore_entry(6,18,2,!(PosY), Hard);
        	view_highscore_entry(6,19,3,!(PosY), Hard);  

        	// EASY
			view_highscore_entry(6,9,1,1, Easy);
        	view_highscore_entry(6,10,2,1, Easy);
        	view_highscore_entry(6,11,3,1, Easy);  

			// Medium
        	view_highscore_entry(6,13,1,1, Medium);
        	view_highscore_entry(6,14,2,1, Medium);
        	view_highscore_entry(6,15,3,1, Medium);  

		}

		if (PosY)  {
		  mode = PM_HoF_edit;
		  // read_eep_block(Difficulty);	
		  copy_buf(&dmyhighscore[0],&ebs.data[0],30);	
		  /*
		  dbg_highscore_print(0, 1);
		  dbg_highscore_print(1, 2);
		  dbg_highscore_print(2, 3);
		  */	
		  Print(8,21,PSTR("ENTER YOUR NAME"));	
		  Print(6,22,PSTR("AND PRESS BUTTON A"));	
		} 

		#ifdef _Fading
		  FadeIn(1, true);
		#endif
		break;	  


	}
	program_mode = mode;

}








void copy_buf(unsigned char *BUFA, unsigned char *BUFB, unsigned char ucANZ)
// copy ucANZ Bytes from buffer BUFA to buffer BUFB
{
 for(;ucANZ>0 ; ucANZ--)
 {
  *(BUFB++) = *(BUFA++);
 }   
}



void fill_buf(unsigned char *BUFA, unsigned char value, unsigned char ucANZ)
// fill ucANZ count of bytes of buffer BUFA with value
{
 for(;ucANZ>0 ; ucANZ--)
 {
  *(BUFA++) = value;
 }   
}


void save_background(u8 x1, u8 y1,u8 x2, u8 y2) {
// save the background in defined area
u8 xi;
  for(;y1 <= y2; y1++) {
    for(xi = x1;xi <= x2; xi++) {
	  menu_background[((y2-y1) * 8) + (x2-xi)] = GetTile(xi,y1);
	}
  }
}


void restore_background(u8 x1, u8 y1,u8 x2, u8 y2) {
// restore the background in defined area
u8 xi;
  for(;y1 <= y2; y1++) {
    for(xi = x1;xi <= x2; xi++) {
	  SetTile(xi,y1, menu_background[((y2-y1) * 8) + (x2-xi)]);	   
	}
  }
}


void msg_window(u8 x1, u8 y1, u8 x2, u8 y2, bool background, bool shadow) {
// draw a window with frame and black backgound on the screen

    // window backgound
	if (background == true) {
	  	Fill(x1 + 1, y1 + 1, x2 - x1 - 1, y2 - y1 - 1,BG_Brown);
    }
    // window shadow
	if ((shadow == true) && (y2 < 27) && (x2 < 29)) {
		// lower shadow
   		Fill(x1 + 1, y2 + 1, x2 - x1 + 1, 1,GB_Black);
  		// right shadow
		Fill(x2 + 1, y1 + 1, 1 , y2 - y1,GB_Black);
 	}	
	// upper frame
	Fill(x1 + 1, y1, x2 - x1 - 1, 1,frame_horizontal);
	// lower frame
	Fill(x1 + 1, y2, x2 - x1 - 1, 1,frame_horizontal);
	// left frame
	Fill(x1 , y1 + 1, 1, y2 - y1 - 1,frame_vertical);
	// right frame
	Fill(x2, y1 + 1, 1 , y2 - y1 - 1,frame_vertical);
	// upper, left corner
	SetTile(x1,y1,frame_upper_left);
	// upper, right corner
	SetTile(x2,y1,frame_upper_right);
	// lower, left corner
	SetTile(x1,y2,frame_lower_left);
	// lower, right corner
	SetTile(x2,y2,frame_lower_right);
}	



void create_new_playing_field(u8 uDiff) {
// creates a new randomized playing field
  char u1,u2,u3,u4;
  u8 u5;

  // clear playing field
  fill_buf(&playing_field[0],Empty,max_pipes);	

  // add start point
  u1 = PRNG_NEXT() % 4; // direction of start point
  u2 = PRNG_NEXT() % 49;// position of start point

  u3 = u2 / 7; 			// y position of start point
  u2 = u2 % 7;			// x position of start point
  	
  playing_field[((u3 + 1) * 9) + u2 + 1] = Start_left_empty + u1;

  Start_point_X = u2 + 1;
  u5 = Start_point_Y = u3 + 1;
  next_pipe(true);
  //Start_point_Y = u5; 

  if (uDiff == Easy) return; 

  // add end point
  do 
  {	  
  	u2 = PRNG_NEXT() % 49;  // position of end point
    u3 = u2 / 7; 			// y position of end point
    u2 = u2 % 7;			// x position of end point
  //}while(check_pipe_area(u2,u3));	
  }while((abs(Start_point_X - 1 - u2) + abs(Start_point_Y - 1 - u3)) < 3);    

  u1 = PRNG_NEXT() % 4; // direction of end point
  playing_field[((u3 + 1) * 9) + u2 + 1] = End_left_empty + u1;

  End_point_X = u2 + 1;
  End_point_Y = u3 + 1;
   

  // add pump(s)
  for(u4 = 0; u4 < uDiff; u4++) {	
  	do 
  	{	  
  		u2 = PRNG_NEXT() % 49;  // position of pump
    	u3 = u2 / 7; 			// y position of pump
    	u2 = u2 % 7;			// x position of pump
  	}while(check_pipe_area(u2,u3));	

  	u1 = PRNG_NEXT() % 2; // direction of pump
  	playing_field[((u3 + 1) * 9) + u2 + 1] = Pump_ver_empty + u1;	
  }

  // add reservoirs
  for(u4 = 0; u4 < uDiff; u4++) {	
  	do 
  	{	  
  		u2 = PRNG_NEXT() % 49;  // position of reservoir
    	u3 = u2 / 7; 			// y position of reservoir
    	u2 = u2 % 7;			// x position of reservoir
  	}while(check_pipe_area(u2,u3));	

  	u1 = PRNG_NEXT() % 2; // direction of reservoir
  	playing_field[((u3 + 1) * 9) + u2 + 1] = Reservoir_ver_empty + u1;	
  }  


  if (uDiff != Hard) return;
  // add horizontal tunnel
  u2 = PRNG_NEXT() % 9; // position of horizontal tunnel
  SetTile(7,(u2 * 2) + 7,GB_Black); SetTile(26,(u2 * 2) + 7,GB_Black); 
  SetTile(7,(u2 * 2) + 8,GB_Black); SetTile(26,(u2 * 2) + 8,GB_Black); 
  Tunnel_hor = u2;   



  // add vertical tunnel
  u2 = PRNG_NEXT() % 9; // position of vertival tunnel
  SetTile((u2 * 2) + 8,6,GB_Black); SetTile((u2 * 2) + 8,25,GB_Black); 
  SetTile((u2 * 2) + 9,6,GB_Black); SetTile((u2 * 2) + 9,25,GB_Black);  
  Tunnel_ver = u2;   
}



u8 check_pipe_area(u8 uX, u8 uY) {
/* check the positions 1 - 8 and tile position 9 for other tiles
 *    
 * 	   123
 *     495  
 *	   678	
 */
  if (playing_field[((uY + 1) * 9) + uX + 1]) return 9; // position 9
  if (playing_field[(uY * 9) + uX]) return 1; // position 1
  if (playing_field[(uY * 9) + uX + 1]) return 2; // position 2
  if (playing_field[(uY * 9) + uX + 2]) return 3; // position 3
  if (playing_field[((uY + 1) * 9) + uX]) return 4; // position 4
  if (playing_field[((uY + 1) * 9) + uX + 2]) return 5; // position 5
  if (playing_field[((uY + 2) * 9) + uX]) return 6; // position 6
  if (playing_field[((uY + 2) * 9) + uX + 1]) return 7; // position 7
  if (playing_field[((uY + 2) * 9) + uX + 2]) return 8; // position 8
  return(0);
}


int GetTile(u8 xx, u8 yy) {
// get tile from VRAM for cursor functions
int i1 = (yy * 60) + (xx * 2);

  return(((vram[i1 + 1] * 256) + vram[i1] - (int)(BGTiles)) / 64);
}


u8 get_pipe(u8 uX, u8 uY) {
// get pipe type from playing field
	return(playing_field[(uY  * 9) + uX]);
}


void set_pipe(u8 uX, u8 uY, u8 uPipe) {
// set pipe type to playing field
	playing_field[(uY  * 9) + uX] = uPipe;
}


void place_pipe(u8 uX, u8 uY, u8 uPipe) {
// place a pipe at playing field
	DrawMap((uX * 2) + 8,(uY * 2) + 7,pipe_maps[uPipe]); 

}


void set_pipe_cursor(u8 xx, u8 yy) {
// set and animate cursor
  // cursor invisible?
  ani_cur++;
  // restore background at old cursor position 
  hide_pipe_cursor(PosX,PosY);   
  // save coordinates	
  PosX = xx;
  PosY = yy;
  if (ani_cur > 10) ani_cur = 0; 
  //if (ani_cur > 5) return;	// counter for animated sprite frame

  // set cursor pipe
  place_pipe(xx,yy,pipe_stack[0].CursorMap);
}


void hide_pipe_cursor(u8 xx, u8 yy) {
// hide the cursor
  if ((xx < 9) & (yy < 9)) place_pipe(xx,yy,get_pipe(xx,yy));   
 
}



void init_pipe_stack(u8 uDiff) {
// init the pipe stack depending the uDiff
u8 u1;
	for(u1 = 0; u1 < 5; u1++) {
	 	pipe_stack_random_map(uDiff,u1);

  	}
}



void show_pipe_stack(void) {
// shows the actual pipe stack	
u8 u1;
	for(u1 = 0; u1 < 5; u1++) {
    	DrawMap(2,(u1 * 2) + 1,pipe_maps[pipe_stack[4 - u1].StackMap]);	
	}
}



void cycle_pipe_stack(u8 uDiff) {
// cycle the actual pipe stack and add a new entry
u8 u1;
	for(u1 = 0; u1 < 4; u1++) {
    	pipe_stack[u1].StackMap = pipe_stack[u1 + 1].StackMap;	
		pipe_stack[u1].GameMap = pipe_stack[u1 + 1].GameMap;
		pipe_stack[u1].CursorMap = pipe_stack[u1 + 1].CursorMap;	
	}	
	pipe_stack_random_map(uDiff,4);


}


void pipe_stack_random_map(u8 uDiff, u8 index) {
// calculate full random pipe for pipe stack
const u8 full_pipes[11] = {
	Straight_ver_full,
	Straight_hor_full,
	Edge_upper_left_full,
	Edge_lower_left_full,
	Edge_upper_right_full,
	Edge_lower_right_full,
	Cross_hor_full_ver_full,
	Arrow_right_full,
	Arrow_left_full,
	Arrow_up_full,
	Arrow_down_full,
	};


const u8 empty_pipes[11] = {
	Straight_ver_empty,
	Straight_hor_empty,
	Edge_upper_left_empty,
	Edge_lower_left_empty,
	Edge_upper_right_empty,
	Edge_lower_right_empty,
	Cross_hor_empty_ver_empty,
	Arrow_right_empty,
	Arrow_left_empty,
	Arrow_up_empty,
	Arrow_down_empty,
	};


	u8 random_pipe, u1, u2;
   	do {
		if (uDiff == Hard)  random_pipe = PRNG_NEXT() % 17;	
		else random_pipe = PRNG_NEXT() % 7;
		u2 = 0; 
		if (random_pipe > 10) random_pipe = random_pipe - 10;
		
		// check for double pipes in stack
		for(u1 = 0; u1 < 5; u1++) {
			if (pipe_stack[u1].StackMap == full_pipes[random_pipe]) u2++;
		}
	} while(u2 > 1);

	pipe_stack[index].StackMap = full_pipes[random_pipe];
	pipe_stack[index].GameMap = empty_pipes[random_pipe];	
	pipe_stack[index].CursorMap = Cursor_straight_ver + random_pipe;
	
}


u8 next_pipe(bool bStart){
// check the next pipe according Flow_direction and position


    if (bStart == false) {		
		// check boarders first
		switch (Flow_direction)
		{
			case flow_right:
			    if ((Start_point_X >= 8) && (Tunnel_hor == Start_point_Y) && (Difficulty == Hard)) {					
					Start_point_X = 0; 
					iSCORE += 80;
					break;
				}
		  		if (Start_point_X >= 8) return(1);
				else Start_point_X = Start_point_X + 1;
				break;

			case flow_left:
			    if ((Start_point_X == 0) && (Tunnel_hor == Start_point_Y) && (Difficulty == Hard)) {					
					Start_point_X = 8; 
					iSCORE += 80;
					break;
				}
		  		if (Start_point_X == 0) return(2);
				else Start_point_X = Start_point_X - 1;
				break;

			case flow_down:
			    if ((Start_point_Y >= 0) && (Tunnel_ver == Start_point_X) && (Difficulty == Hard)) {					
					Start_point_Y = 0; 
					iSCORE += 80;
					break;
				}		  		
				if (Start_point_Y >= 8) return(3);
				else Start_point_Y = Start_point_Y + 1;
				break;

			case flow_up:
			    if ((Start_point_Y == 0) && (Tunnel_ver == Start_point_X) && (Difficulty == Hard)) {					
					Start_point_Y = 8; 
					iSCORE += 80;
					break;
				}
		  		if (Start_point_Y == 0) return(4);
				else Start_point_Y = Start_point_Y - 1;
				break;
		}
	}

	/*	
    PrintByte(5,22,Start_point_X,true);
	PrintByte(5,23,Start_point_Y,true);
	PrintByte(5,24,get_pipe(Start_point_X,Start_point_Y),true);
	*/

	// reset flow_speed after a reservoir to default value
	if ((Flow_speed > Flow_Speed_Easy) || (pump_cnt == 1)) set_flow_speed();
	if (pump_cnt) pump_cnt--;
		
	// check next pipe
	// state machine
	switch(get_pipe(Start_point_X,Start_point_Y)) 
	{		

		case Start_left_empty:
		  if ((Flow_direction != flow_left) && (bStart == false)) return(5);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_start_piece_left[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  //set_pipe(Start_point_X,Start_point_Y,Start_left_full);
		  // init flow direction
		  Flow_direction = flow_left;
    	  break;
			

		case Start_right_empty:
		  if ((Flow_direction != flow_right) && (bStart == false)) return(6);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_start_piece_right[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  //set_pipe(Start_point_X,Start_point_Y,Start_right_full);
		  // init flow direction
		  Flow_direction = flow_right;
  		  break;


		case Start_up_empty:
		  if ((Flow_direction != flow_up) && (bStart == false)) return(7);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_start_piece_up[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  //set_pipe(Start_point_X,Start_point_Y,Start_up_full);
		  // init flow direction
		  Flow_direction = flow_up;		
		  break;


		case Start_down_empty:
		  if ((Flow_direction != flow_down) && (bStart == false)) return(8);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_start_piece_down[0], sizeof(Flow_animation) );	
		  // refresh playing field 
		  //set_pipe(Start_point_X,Start_point_Y,Start_down_full);
		  // init flow direction
		  Flow_direction = flow_down;		  
		  break;


		case Straight_ver_empty:	
		  if ((Flow_direction == flow_left) || (Flow_direction == flow_right)) return(9);
          else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_straight_tube_up[0], sizeof(Flow_animation) );			
          } else {		    
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_straight_tube_down[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Straight_ver_full);	
		  iSCORE += 5;	   
		  if (Distance) Distance--;
		  break;

		case Straight_hor_empty:	
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_down)) return(10);
          else if (Flow_direction == flow_right) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_straight_tube_right[0], sizeof(Flow_animation) );			
          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_straight_tube_left[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Straight_hor_full);		   		   
		  iSCORE += 5;
		  if (Distance) Distance--;
		  break;


		case Edge_upper_left_empty:	
		  if ((Flow_direction == flow_left) || (Flow_direction == flow_up)) return(11);
          else if (Flow_direction == flow_down) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_ul_down[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_left;
          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_ul_right[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_up;
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Edge_upper_left_full);
		  iSCORE += 5;		   
		  if (Distance) Distance--;
		  break;

		case Edge_lower_left_empty:	
		  if ((Flow_direction == flow_left) || (Flow_direction == flow_down)) return(12);
          else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_dl_up[0], sizeof(Flow_animation));			
			// set new flow direction
			Flow_direction = flow_left;
          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_dl_right[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_down;
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Edge_lower_left_full);		   
		  iSCORE += 5;
		  if (Distance) Distance--;
		  break;

		case Edge_upper_right_empty:	
		  if ((Flow_direction == flow_right) || (Flow_direction == flow_up)) return(13);
          else if (Flow_direction == flow_down) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_ur_down[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_right;
          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_ur_left[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_up;
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Edge_upper_right_full);		   
		  iSCORE += 5;
		  if (Distance) Distance--;
		  break;

		case Edge_lower_right_empty:	
		  if ((Flow_direction == flow_right) || (Flow_direction == flow_down)) return(14);
          else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_dr_up[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_right;
          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_edge_dr_left[0], sizeof(Flow_animation) );			
			// set new flow direction
			Flow_direction = flow_down;
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Edge_lower_right_full);	
		  iSCORE += 5;	   
		  if (Distance) Distance--;
		  break;


		case Cross_hor_empty_ver_empty:	
		  if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_he_up[0], sizeof(Flow_animation) );			
		    // refresh playing field 
		    set_pipe(Start_point_X,Start_point_Y,Cross_hor_empty_ver_full);		   

	      } else if (Flow_direction == flow_down) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_he_down[0], sizeof(Flow_animation) );			
		    // refresh playing field 
		    set_pipe(Start_point_X,Start_point_Y,Cross_hor_empty_ver_full);		   

	      } else if (Flow_direction == flow_right) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_ve_right[0], sizeof(Flow_animation) );			
		    // refresh playing field 
		    set_pipe(Start_point_X,Start_point_Y,Cross_hor_full_ver_empty);		   

          } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_ve_left[0], sizeof(Flow_animation) );			
		    // refresh playing field 
		    set_pipe(Start_point_X,Start_point_Y,Cross_hor_full_ver_empty);		   

          }		  
		  iSCORE += 5;
		  if (Distance) Distance--;
		  break;


		case Cross_hor_full_ver_empty:	
	      if ((Flow_direction == flow_right) || (Flow_direction == flow_left)) return(15);
		  else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_hf_up[0], sizeof(Flow_animation) );			

	      } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_hf_down[0], sizeof(Flow_animation) );			
		  }
  	      // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Cross_hor_full_ver_full);		
		  loop_cnt++;   
		  iSCORE += (20 * loop_cnt);
		  if (Distance) Distance--;
		  break;


		case Cross_hor_empty_ver_full:	
	      if ((Flow_direction == flow_up) || (Flow_direction == flow_down)) return(16);
		  else if (Flow_direction == flow_right) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_vf_right[0], sizeof(Flow_animation) );			

	      } else {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pipe_cross_vf_left[0], sizeof(Flow_animation) );			
		  }
  	      // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Cross_hor_full_ver_full);
		  loop_cnt++;		   
		  iSCORE += (20 * loop_cnt);
		  if (Distance) Distance--;
		  break;


		case Arrow_right_empty:	
		  if (Flow_direction != flow_right) return(17);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_pipe_arrow_right[0], sizeof(Flow_animation) );			          	  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Arrow_right_full);	
		  iSCORE += 10;	   
		  if (Distance) Distance--;
		  break;


		case Arrow_left_empty:	
		  if (Flow_direction != flow_left) return(18);
          // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_pipe_arrow_left[0], sizeof(Flow_animation) );			
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Arrow_left_full);	
		  iSCORE += 10;	   
		  if (Distance) Distance--;
		  break;


		case Arrow_down_empty:	
		  if (Flow_direction != flow_down) return(19);          
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_pipe_arrow_down[0], sizeof(Flow_animation) );			
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Arrow_down_full);	
		  iSCORE += 10;	   
		  if (Distance) Distance--;
		  break;


		case Arrow_up_empty:	
		  if (Flow_direction != flow_up) return(20);
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_pipe_arrow_up[0], sizeof(Flow_animation) );			
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Arrow_up_full);	
		  iSCORE += 10;	   
		  if (Distance) Distance--;
		  break;


		case Reservoir_ver_empty:	
		  if ((Flow_direction == flow_left) || (Flow_direction == flow_right)) return(21);
          else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_reservoir_up[0], sizeof(Flow_animation) );			
          } else {		    
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_reservoir_down[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Reservoir_ver_full);	
		  iSCORE += 20;	   
		  Flow_speed = Flow_speed * 2;  
		  if (Distance) Distance--;
		  break;


		case Reservoir_hor_empty:	
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_down)) return(22);
          else if (Flow_direction == flow_right) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_reservoir_right[0], sizeof(Flow_animation) );			
          } else {		    
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_reservoir_left[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Reservoir_hor_full);	
		  iSCORE += 20;	   
		  Flow_speed = Flow_speed * 2;  
		  if (Distance) Distance--;
		  break;

		case Pump_ver_empty:	
		  if ((Flow_direction == flow_left) || (Flow_direction == flow_right)) return(23);
          else if (Flow_direction == flow_up) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pump_up[0], sizeof(Flow_animation) );			
          } else {		    
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pump_down[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Pump_ver_full);	
		  iSCORE += 100;	 
		  Flow_speed = Flow_speed / 2;  
		  pump_cnt=6;
		  if (Distance) Distance--;
		  break;


		case Pump_hor_empty:	
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_down)) return(24);
          else if (Flow_direction == flow_right) {
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pump_right[0], sizeof(Flow_animation) );			
          } else {		    
		  	// load current pipe from animation
		  	memcpy_P( &Flow_animation[0], &ani_pump_left[0], sizeof(Flow_animation) );			
          }		  
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,Pump_hor_full);	
		  iSCORE += 100;	   
		  Flow_speed = Flow_speed / 2;  
		  pump_cnt=6;
		  if (Distance) Distance--;
		  break;

		case End_left_empty:
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_down) || (Flow_direction == flow_left)) return(25);	
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_end_piece_right[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,End_left_full);
		  // init flow direction
		  Flow_direction = flow_left;
 		  iSCORE *= 2;
  	  	  break;
			

		case End_right_empty:
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_down) || (Flow_direction == flow_right)) return(26);	
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_end_piece_left[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,End_right_full);
		  // init flow direction
		  Flow_direction = flow_right;
		  iSCORE *= 2;
  		  break;


		case End_up_empty:
		  if ((Flow_direction == flow_up) || (Flow_direction == flow_right) || (Flow_direction == flow_left)) return(27);	
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_end_piece_down[0], sizeof(Flow_animation) );		
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,End_up_full);
		  // init flow direction
		  Flow_direction = flow_up;		
		  iSCORE *= 2;
		  break;


		case End_down_empty:
		  if ((Flow_direction == flow_down) || (Flow_direction == flow_right) || (Flow_direction == flow_left)) return(28);	
		  // load current pipe from animation
		  memcpy_P( &Flow_animation[0], &ani_end_piece_up[0], sizeof(Flow_animation) );	
		  // refresh playing field 
		  set_pipe(Start_point_X,Start_point_Y,End_down_full);
		  // init flow direction
		  Flow_direction = flow_down;		  
		  iSCORE *= 2;
		  break;



        default:
		  return(29);


	} 

	return(0);
}


void end_game(void) {
// check the playing field for unused pipes
u8 uX, uY, u1, uStep;

	for(uX = 0; uX < 9; uX++)
	for(uY = 0; uY < 9; uY++) {
		u1 = get_pipe(uX,uY);	
		if ((u1 == Straight_ver_empty) || (u1 == Straight_hor_empty) || (u1 == Edge_upper_left_empty) ||
		   (u1 == Edge_lower_right_empty) || (u1 == Edge_lower_left_empty) || (u1 == Edge_upper_right_empty) ||
		   (u1 == Cross_hor_empty_ver_empty) || (u1 == Arrow_right_empty) || (u1 == Arrow_left_empty) ||
		   (u1 == Arrow_up_empty) || (u1 == Arrow_down_empty)) {
				TriggerFx(5, 0x7f, true);
				for(uStep = 13; uStep;) {
					animate_explosion(&uX,&uY,&uStep,true);
					WaitVsync(2);
				}
				if ((iSCORE) && (iSCORE < 10))	iSCORE = 0;
				else if (iSCORE >= 10)	iSCORE -= 10;
				PrintInt(20,2,iSCORE,false);


		}
	}
	set_pipe(PosX,PosY,Empty);
	place_pipe(PosX,PosY,Empty);
	if ((Distance) && (Wrenches)) {
	 	Wrenches--;
		if (!(Wrenches)) Print(9,3,PSTR("    GAME OVER"));
		
 	}

}



void Draw_Wrench_reserve(void)
// draw reserve of wrenches
{
  for(u8 uc1=0; uc1 < 3; uc1++) {
  	// draw wrenches
    if (uc1 < Wrenches) PrintChar(18 + uc1,3,'&');
	// draw plain background
	else PrintChar(18 + uc1,3,' ');
  }

}


void Draw_Intro_char(u8 uX, u8 uY, const int *PChar, u8 columns) {
// draws a pipe char for the intro screen
u8 xx, yy;

	//memcpy_P( &Flow_animation[0], *PChar, sizeof(Flow_animation) );	
	for(xx=0; xx < columns ; xx++)
	for(yy=0; yy < 5; yy++) DrawMap((xx * 2) + uX,(yy * 2) + uY,(const int *)(PChar[xx + (yy * columns)]));


}


void set_flow_speed(void) {
// set flow speed according difficulty
	if (Difficulty == Easy) {
	    Flow_speed = Flow_Speed_Easy;
	} else if (Difficulty == Medium) {
		Flow_speed = Flow_Speed_Medium;	
	} else {
		Flow_speed = Flow_Speed_Hard;	
	}
}


/*** Animations *********************************************************************************/

void animate_pipes(void) {
// animate current pipe	
    u8 uback;
	// draw current pipe	
	DrawMap((Start_point_X * 2) + 8,(Start_point_Y * 2) + 7,(const int *)(Flow_animation[Flow_ani_cnt]));
	
	// increment animation counter	
	Flow_ani_cnt++;
    if (Flow_ani_cnt == 4) { 
	  uback = next_pipe(false);
	  //if (next_pipe(&Start_point_X,&Start_point_Y,false)) {
	  if (uback) {
	  	/*
		PrintByte(5,16,Start_point_X,true);
		PrintByte(5,17,Start_point_Y,true);		
		PrintByte(5,18,Flow_direction,true);
		PrintByte(5,19,uback,true);
		*/
		WaitVsync(30);

	  	program_mode = PM_Game_End;
		end_game();
		hide_pipe_cursor(PosX,PosY);
		msg_window(8,10,24,16,true,true);	
		Print(10,12,PSTR("PRESS ANY KEY"));
		Print(10,13,PSTR("     FOR"));
		Print(10,14,PSTR(" A NEW GAME!"));
		TriggerFx(6, 0x7f, true);
      } 
	  Flow_ani_cnt = 0;
	}

}


void animate_start(u8 uDiff) {
// animate start and end sign befor water flow

	if (Flow_ani_cnt & 0x01) {
	    if (!((Start_point_X == PosX) && (Start_point_Y == PosY))) {
			// restore start_point		
			DrawMap((Start_point_X * 2) + 8,(Start_point_Y * 2) + 7,pipe_maps[get_pipe(Start_point_X,Start_point_Y)]);
        }

	    if (!((End_point_X == PosX) && (End_point_Y == PosY))) {
			// restore end_point
			if (uDiff != Easy) DrawMap((End_point_X * 2) + 8,(End_point_Y * 2) + 7,pipe_maps[get_pipe(End_point_X,End_point_Y)]);
	    }

	} else {
	    if (!((Start_point_X == PosX) && (Start_point_Y == PosY))) {
		   // restore start_point
		   SetTile((Start_point_X * 2) + 8,(Start_point_Y * 2) + 7,Start_sign);
        }

	    if (!((End_point_X == PosX) && (End_point_Y == PosY))) {
		   // restore end_point
		   if (uDiff != Easy) SetTile((End_point_X * 2) + 8,(End_point_Y * 2) + 7,End_sign);				
        }
	}
    
	// progress bar
	SetTile(27,Flow_ani_cnt + 6,GB_Black);
	

	// increment animation counter	
	Flow_ani_cnt++;
    
	if (Flow_ani_cnt == 20) { 
	  program_mode = PM_Game_Flow;
	  Flow_ani_cnt = 0;
	  TriggerFx(7, 0x7f, true);
	}

}


void animate_explosion(u8 *uX,u8 *uY, u8 *Step, bool BEndGame) {
// animate explosion	
	
	if (*Step) {    
	   *Step = *Step - 1;
	   DrawMap((*uX * 2) + 8,(*uY * 2) + 7,ani_pipe_explosion[ani_pipe_explosion_max - (*Step / 2 )]);
	   if ((*Step == 0) && !(BEndGame)){
		  place_pipe(*uX,*uY,get_pipe(*uX,*uY));	 			     	   
	   
	   }			  		
	}
}

	



int get_highscore(u8 entry) {
// get the actual highscore from eeprom
  // check the value for entry	
  if (entry > 2) return(0);
	
  // read the eeprom block
  if (!read_eep_block(Difficulty)) return(0);	
 
  if (Difficulty == Medium) return((ebs.data[(entry * 5)+18] * 256) + ebs.data[(entry * 5)+19]);
  return((ebs.data[(entry * 5)+3] * 256) + ebs.data[(entry * 5)+4]);
}



u8 check_highscore(void) {
// check the actual highsore
u8 a;
int i1;
  // read the eeprom block
  if (!read_eep_block(Difficulty)) return(0);	

  if (Difficulty == Medium) {
  	for(a=0; a<3; a++) {
    	i1 = (ebs.data[(a * 5)+18] * 256) + ebs.data[(a * 5)+19];
    	if (iSCORE > i1) return(a + 1);
  	}

  } else {
	// difficulty easy and hard
  	for(a=0; a<3; a++) {
    	i1 = (ebs.data[(a * 5)+3] * 256) + ebs.data[(a * 5)+4];
    	if (iSCORE > i1) return(a + 1);
  	}  
  
  }	
  // highscore is lower as saved highscores 
  return(0);
}



void copy_highsore(u8 entry_from, u8 entry_to) {
// copy a highscore entry to another slot


  if (Difficulty == Medium) copy_buf(&ebs.data[15 + (entry_from * 5)],&ebs.data[15 + (entry_to * 5)],5);
  else copy_buf(&ebs.data[entry_from * 5],&ebs.data[entry_to * 5],5);
   
}



void clear_highsore(u8 entry) {
// clear the name in actual entry and set the score to highscore
u8 a;
  if (entry > 2) return;
  
  if (Difficulty == Medium) {
  	// clear name 
  	for(a=0; a<3; a++) {
    	ebs.data[(entry * 5) + a + 15] = 'A';
  	}   
  	// set score
  	ebs.data[(entry * 5) + 18] = iSCORE / 256;
  	ebs.data[(entry * 5) + 19] = iSCORE % 256;


  } else {
  	// clear name 
  	for(a=0; a<3; a++) {
    	ebs.data[(entry * 5) + a] = 'A';
  	}   
  	// set score
  	ebs.data[(entry * 5) + 3] = iSCORE / 256;
  	ebs.data[(entry * 5) + 4] = iSCORE % 256;
  } 	
}



u8 set_def_highscore(void) {
// write the default highscore list in the EEPROM
  u8 empty_data_set[5] = {'-','-','-',0x00,0x00};
  // difficulty easy / hard	
  // entry 1 
  copy_buf(&empty_data_set[0],&ebs.data[0],5);
  // entry 2 
  copy_buf(&empty_data_set[0],&ebs.data[5],5);
  // entry 3 
  copy_buf(&empty_data_set[0],&ebs.data[10],5);
  	
  // difficulty medium
  // entry 1 
  copy_buf(&empty_data_set[0],&ebs.data[15],5);
  // entry 2 
  copy_buf(&empty_data_set[0],&ebs.data[20],5);
  // entry 3 
  copy_buf(&empty_data_set[0],&ebs.data[25],5);
  return(EepromWriteBlock(&ebs));	  
}



u8 view_highscore_entry(u8 x, u8 y, u8 entry, u8 load_data, u8 uDiff) {
// shows an entry of the higscore
u8 a,c;

  // read the eeprom block
  if (load_data) {
  	// read_eep_block(uDiff);
	if (uDiff == Hard) a = 26;
	else a = 25;
	if (!isEepromFormatted() || EepromReadBlock(a, &ebs))
        return(1);   
	
  }

  entry--;

  if (uDiff == Medium) {
  	// print name
  	for(a = 0; a < 3;a++) {
		c = ebs.data[a + (entry * 5) + 15];  
		PrintChar(16 + x + a, y, c);  
  	}
  	// print score
  	PrintInt(x + 12, y, (ebs.data[(entry * 5) + 18] * 256) + ebs.data[(entry * 5) + 19], true);
	Print(x, y,PSTR("MEDIUM"));
  
  } else { 	
  	// print name
  	for(a = 0; a < 3;a++) {
		c = ebs.data[a + (entry * 5)];  
		PrintChar(16 + x + a, y, c);  
  	}
  	// print score
  	PrintInt(x + 12, y, (ebs.data[(entry * 5) + 3] * 256) + ebs.data[(entry * 5) + 4], true);
	if (uDiff == Easy) Print(x, y,PSTR("EASY"));
	else Print(x, y,PSTR("HARD"));
  }

  return(0);
}




u8 read_eep_block(u8 uDiff) {
// read the eeprom block depending uDiff

  if (uDiff == Hard) {
  	if (!isEepromFormatted() || EepromReadBlock(26, &ebs))
        return(0);   
  } else {
  	if (!isEepromFormatted() || EepromReadBlock(25, &ebs))
        return(0);     
  }   
  
  // ok
  return(1);

}



void edit_highscore_entry(u8 entry, u8 cursor_pos, u8 b_mode) {
// edit and view and char in the name of choosed entry    
  u8 c;
  entry--;
  
  if (Difficulty == Medium) c = ebs.data[(entry * 5) + 15 + cursor_pos];
  else c = ebs.data[(entry * 5) + cursor_pos];
  // proceed up & down button
  if (b_mode == BTN_UP) {
     c++;
     if (c > 'Z') c = ' '; 
     else if (c == '!') c = 'A';
  }
  if (b_mode == BTN_DOWN) {		 
     c--;      
     if (c == 0x1F) c = 'Z';
	 else if (c < 'A') c = ' ';
  }
  if (Difficulty == Medium) ebs.data[(entry * 5) + 15 + cursor_pos] = c;
  else ebs.data[(entry * 5) + cursor_pos] = c;

}


void show_highscore_char(u8 entry, u8 position, u8 cursor_on) {
// shows a char of edited name
    u8 c;
	if (Difficulty == Medium) c = ebs.data[(entry * 5) + 15 + position];
	else c = ebs.data[(entry * 5) + position];
    if (cursor_on) PrintChar(22 + position, entry + 9 + (Difficulty * 4), '-');   // show '_'
    else PrintChar(22 + position, entry + 9 + (Difficulty * 4), c); 	
}


/*
void dbg_highscore_print(u8 entry, u8 position) {
// debug print
    u8 c;
	for(c=0; c < 5; c++) {
	   if (Difficulty == Medium) PrintHexByte((c * 3), position, ebs.data[(entry * 5) + 15 + c]);
	   else PrintHexByte((c * 3), position, ebs.data[(entry * 5) + c]);
	}
}
*/


