/*
 *  animations for Pipe dream  
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


/*
 *** start point ***************************************************************************
*/

#define ani_start_piece_right_max 3
const int *ani_start_piece_right[] PROGMEM = {
	start_piece_r1,
	start_piece_r1,
	start_piece_r2,
	start_piece_rf
};


#define ani_start_piece_left_max 3
const int *ani_start_piece_left[] PROGMEM = {
	start_piece_l1,
	start_piece_l1,
	start_piece_l2,
	start_piece_lf
};


#define ani_start_piece_up_max 3
const int *ani_start_piece_up[] PROGMEM = {
	start_piece_u1,
	start_piece_u2,
	start_piece_uf,
	start_piece_uf
};


#define ani_start_piece_down_max 3
const int *ani_start_piece_down[] PROGMEM = {
	start_piece_d1,
	start_piece_d2,
	start_piece_df,
	start_piece_df
};

/*
 *** end point ***************************************************************************
*/

#define ani_end_piece_right_max 3
const int *ani_end_piece_right[] PROGMEM = {
	end_piece_r1,
	end_piece_r2,
	start_piece_lf,
	start_piece_lf
};


#define ani_end_piece_left_max 3
const int *ani_end_piece_left[] PROGMEM = {
	end_piece_l1,
	end_piece_l2,
	start_piece_rf,
	start_piece_rf
};


#define ani_end_piece_up_max 3
const int *ani_end_piece_up[] PROGMEM = {
	end_piece_u1,
	end_piece_u2,
	start_piece_df,
	start_piece_df
};


#define ani_end_piece_down_max 3
const int *ani_end_piece_down[] PROGMEM = {
	end_piece_d1,
	end_piece_d2,
	start_piece_uf,
	start_piece_uf
};


/*
 *** reservoir *****************************************************************************
*/

#define ani_reservoir_right_max 4
const int *ani_reservoir_right[] PROGMEM = {
	reservoir_r1,
	reservoir_r2,
	reservoir_r3,
	reservoir_hf
};


#define ani_reservoir_left_max 4
const int *ani_reservoir_left[] PROGMEM = {
	reservoir_l1,
	reservoir_l2,
	reservoir_l3,
	reservoir_hf
};


#define ani_reservoir_up_max 4
const int *ani_reservoir_up[] PROGMEM = {
	reservoir_u1,
	reservoir_u2,
	reservoir_u3,
	reservoir_vf
};


#define ani_reservoir_down_max 4
const int *ani_reservoir_down[] PROGMEM = {
	reservoir_d1,
	reservoir_d2,
	reservoir_d3,
	reservoir_vf
};


/*
 *** pump ********************************************************************************
*/

#define ani_pump_right_max 4
const int *ani_pump_right[] PROGMEM = {
	pump_r1,
	pump_r2,
	pump_r3,
	pump_hf
};


#define ani_pump_left_max 4
const int *ani_pump_left[] PROGMEM = {
	pump_l1,
	pump_l2,
	pump_l3,
	pump_hf
};


#define ani_pump_up_max 4
const int *ani_pump_up[] PROGMEM = {
	pump_u1,
	pump_u2,
	pump_u3,
	pump_vf
};


#define ani_pump_down_max 4
const int *ani_pump_down[] PROGMEM = {
	pump_d1,
	pump_d2,
	pump_d3,
	pump_vf
};



/*
 *** straight tube *************************************************************************
*/

#define ani_straight_tube_right_max 4
const int *ani_straight_tube_right[] PROGMEM = {
	pipe_straight_r1,
	pipe_straight_r2,
	pipe_straight_r3,
	pipe_straight_hf
};


#define ani_straight_tube_left_max 4
const int *ani_straight_tube_left[] PROGMEM = {
	pipe_straight_l1,
	pipe_straight_l2,
	pipe_straight_l3,
	pipe_straight_hf
};


#define ani_straight_tube_up_max 4
const int *ani_straight_tube_up[] PROGMEM = {
	pipe_straight_u1,
	pipe_straight_u2,
	pipe_straight_u3,
	pipe_straight_vf
};


#define ani_straight_tube_down_max 4
const int *ani_straight_tube_down[] PROGMEM = {
	pipe_straight_d1,
	pipe_straight_d2,
	pipe_straight_d3,
	pipe_straight_vf
};


/*
 *** edge tube *************************************************************************
*/

#define ani_pipe_edge_ul_right_max 4
const int *ani_pipe_edge_ul_right[] PROGMEM = {
	pipe_edge_ul_r1,
	pipe_edge_ul_r2,
	pipe_edge_ul_r3,
	pipe_edge_ul_f
};


#define ani_pipe_edge_ul_down_max 4
const int *ani_pipe_edge_ul_down[] PROGMEM = {
	pipe_edge_ul_d1,
	pipe_edge_ul_d2,
	pipe_edge_ul_d3,
	pipe_edge_ul_f
};


#define ani_pipe_edge_ur_left_max 4
const int *ani_pipe_edge_ur_left[] PROGMEM = {
	pipe_edge_ur_l1,
	pipe_edge_ur_l2,
	pipe_edge_ur_l3,
	pipe_edge_ur_f
};


#define ani_pipe_edge_ur_down_max 4
const int *ani_pipe_edge_ur_down[] PROGMEM = {
	pipe_edge_ur_d1,
	pipe_edge_ur_d2,
	pipe_edge_ur_d3,
	pipe_edge_ur_f
};


#define ani_pipe_edge_dl_right_max 4
const int *ani_pipe_edge_dl_right[] PROGMEM = {
	pipe_edge_dl_r1,
	pipe_edge_dl_r2,
	pipe_edge_dl_r3,
	pipe_edge_dl_f
};


#define ani_pipe_edge_dl_up_max 4
const int *ani_pipe_edge_dl_up[] PROGMEM = {
	pipe_edge_dl_u1,
	pipe_edge_dl_u2,
	pipe_edge_dl_u3,
	pipe_edge_dl_f
};


#define ani_pipe_edge_dr_left_max 4
const int *ani_pipe_edge_dr_left[] PROGMEM = {
	pipe_edge_dr_l1,
	pipe_edge_dr_l2,
	pipe_edge_dr_l3,
	pipe_edge_dr_f
};


#define ani_pipe_edge_dr_up_max 4
const int *ani_pipe_edge_dr_up[] PROGMEM = {
	pipe_edge_dr_u1,
	pipe_edge_dr_u2,
	pipe_edge_dr_u3,
	pipe_edge_dr_f
};


/*
 *** cross tube *************************************************************************
*/

#define ani_pipe_cross_ve_right_max 4
const int *ani_pipe_cross_ve_right[] PROGMEM = {
	pipe_cross_ve_r1,
	pipe_cross_ve_r2,
	pipe_cross_ve_r3,
	pipe_cross_ve_hf
};


#define ani_pipe_cross_ve_left_max 4
const int *ani_pipe_cross_ve_left[] PROGMEM = {
	pipe_cross_ve_l1,
	pipe_cross_ve_l2,
	pipe_cross_ve_l3,
	pipe_cross_ve_hf
};


#define ani_pipe_cross_vf_right_max 4
const int *ani_pipe_cross_vf_right[] PROGMEM = {
	pipe_cross_vf_r1,
	pipe_cross_vf_r2,
	pipe_cross_vf_r3,
	pipe_cross_vf_hf
};


#define ani_pipe_cross_vf_left_max 4
const int *ani_pipe_cross_vf_left[] PROGMEM = {
	pipe_cross_vf_l1,
	pipe_cross_vf_l2,
	pipe_cross_vf_l3,
	pipe_cross_vf_hf
};




#define ani_pipe_cross_he_down_max 4
const int *ani_pipe_cross_he_down[] PROGMEM = {
	pipe_cross_d1_he,
	pipe_cross_d2_he,
	pipe_cross_d2_he,
	pipe_cross_vf_he
};


#define ani_pipe_cross_he_up_max 4
const int *ani_pipe_cross_he_up[] PROGMEM = {
	pipe_cross_u1_he,
	pipe_cross_u1_he,
	pipe_cross_u2_he,
	pipe_cross_vf_he
};


#define ani_pipe_cross_hf_down_max 4
const int *ani_pipe_cross_hf_down[] PROGMEM = {
	pipe_cross_d1_hf,
	pipe_cross_d2_hf,
	pipe_cross_d2_hf,
	pipe_cross_vf_hf
};


#define ani_pipe_cross_hf_up_max 4
const int *ani_pipe_cross_hf_up[] PROGMEM = {
	pipe_cross_u1_hf,
	pipe_cross_u1_hf,
	pipe_cross_u2_hf,
	pipe_cross_vf_hf
};



/*
 *** arrow tube *************************************************************************
*/

#define ani_pipe_arrow_right_max 4
const int *ani_pipe_arrow_right[] PROGMEM = {
	pipe_arrow_right_r1,
	pipe_arrow_right_r2,
	pipe_arrow_right_r3,
	pipe_arrow_right_hf
};


#define ani_pipe_arrow_left_max 4
const int *ani_pipe_arrow_left[] PROGMEM = {
	pipe_arrow_left_l1,
	pipe_arrow_left_l2,
	pipe_arrow_left_l3,
	pipe_arrow_left_hf
};


#define ani_pipe_arrow_up_max 4
const int *ani_pipe_arrow_up[] PROGMEM = {
	pipe_arrow_up_u1,
	pipe_arrow_up_u2,
	pipe_arrow_up_u3,
	pipe_arrow_up_vf
};


#define ani_pipe_arrow_down_max 4
const int *ani_pipe_arrow_down[] PROGMEM = {
	pipe_arrow_dwn_d1,
	pipe_arrow_dwn_d2,
	pipe_arrow_dwn_d3,
	pipe_arrow_dwn_vf
};



/*
 *** pipe explosion *********************************************************************
*/

#define ani_pipe_explosion_max 6
const int *ani_pipe_explosion[] = {
	explosion_1,
	explosion_2,
	explosion_3,
	explosion_2,
	explosion_1,
	empty_field
};


/*
 *** maps for playing field *************************************************************
*/
const int *pipe_maps[] = {
	empty_field,
	start_piece_le,
	start_piece_re,
	start_piece_ue,
	start_piece_de,
	start_piece_lf,
	start_piece_rf,
	start_piece_uf,
	start_piece_df,
	start_piece_le,
	start_piece_re,
	start_piece_ue,
	start_piece_de,
	start_piece_lf,
	start_piece_rf,
	start_piece_uf,
	start_piece_df,
	pump_ve,
	pump_he,
	pump_vf,
	pump_hf,
	reservoir_ve,
	reservoir_he,
	reservoir_vf,
	reservoir_hf,
	pipe_straight_ve,
	pipe_straight_he,
	pipe_straight_vf,
	pipe_straight_hf,
	pipe_edge_ul_e,
	pipe_edge_ul_f,
	pipe_edge_dl_e,
	pipe_edge_dl_f,
	pipe_edge_ur_e,
	pipe_edge_ur_f,
	pipe_edge_dr_e,
	pipe_edge_dr_f,
	pipe_cross_ve_he,
	pipe_cross_ve_hf,
	pipe_cross_vf_he,
	pipe_cross_vf_hf,
	pipe_arrow_right_he,
	pipe_arrow_right_hf,
	pipe_arrow_left_he,
	pipe_arrow_left_hf,
	pipe_arrow_up_ve,
	pipe_arrow_up_vf,
	pipe_arrow_dwn_ve,
	pipe_arrow_dwn_vf,
	cursor_straight_ver,
	cursor_straight_hor,
	cursor_edge_dr,
	cursor_edge_ur,
	cursor_edge_dl,
	cursor_edge_ul,
	cursor_cross,
	cursor_arrow_right,
	cursor_arrow_left,
	cursor_arrow_up,
	cursor_arrow_down
};


/* Char maps for pipe logo */
const int *pipe_logo_P[] = {
	pipe_edge_dr_f,		pipe_straight_hf,	pipe_edge_dl_f,	
	pipe_straight_vf,	empty_intro,		pipe_straight_vf,
	pipe_cross_vf_hf, 	pipe_straight_hf,	pipe_edge_ul_f,	
	pipe_straight_vf,	empty_intro,		empty_intro,	
	pipe_straight_vf,	empty_intro,		empty_intro,

};

const int *pipe_logo_I[] = {
	pipe_straight_vf,	
	pipe_straight_vf,	
	pipe_straight_vf,	
	pipe_straight_vf,	
	pipe_straight_vf,	
};


const int *pipe_logo_E[] = {
	pipe_edge_dr_f,		pipe_straight_hf,	
	pipe_straight_vf,	empty_intro,		
	pipe_cross_vf_hf, 	pipe_straight_hf,	
	pipe_straight_vf,	empty_intro,			
	pipe_edge_ur_f,		pipe_straight_hf,	
};

/*
const int *pipe_logo_S[] = {
	pipe_edge_dr_f,		pipe_straight_hf,	pipe_straight_hf,	
	pipe_straight_vf,	empty_intro,		empty_intro,
	pipe_edge_ur_f, 	pipe_straight_hf,	pipe_edge_dl_f,	
	empty_intro,		empty_intro,		pipe_straight_vf,	
	pipe_straight_hf,	pipe_straight_hf,	pipe_edge_ul_f,
};*/


const int *pipe_logo_S[] = {
	pipe_edge_dr_f,		pipe_straight_hf,	
	pipe_straight_vf,	empty_intro,
	pipe_edge_ur_f, 	pipe_edge_dl_f,	
	empty_intro,		pipe_straight_vf,	
	pipe_straight_hf,	pipe_edge_ul_f,
};

