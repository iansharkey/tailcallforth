
: reset s0 dsp! ;
: literal immediate ['] lit , , ;
: here dp @ ;
: [compile] immediate word (find) >cfa , ;
: recurse immediate latest @ >cfa , ;
: if immediate ['] 0branch , here 0 , ;
: then immediate dup dp @ swap - swap ! ;
: else immediate ['] branch , here 0 , swap dup here swap - swap ! ;
: begin immediate here ;
: until immediate ['] 0branch , here - , ;
: again ['] branch , here - ;
: while immediate ['] 0branch , here swap 0 , ;
: repeat immediate ['] branch , here - , dup here swap - swap ! ;

: char word drop c@ ;

: '(' 40 ;           
: ')' 41 ;


: ( immediate 1 begin key dup '(' = if 
  drop 1+
 else 
 ')' = if
  1-
 then
 then
 dup 0= until
 drop
;


 : nip ( x y -- y ) swap drop ;
 : tuck ( x y -- y x y ) dup rot ;
 : pick 1+ cells * dsp@ + @ ;

: space 32 emit ;

: .s '(' emit space
	      dsp@
	      0 s0 +  cells -
	      begin
	      over over <=
	      while
	      dup @ .
	      space
	      cells -
	      repeat
	      drop drop
	      ')' emit space
		 ;
 
 
: test dup if dup . 1- recurse then 69 . ;


: test2 1 if 5 . else 6 . then 7 . ;


: test 5 begin dup . 1- 0= until ;


