
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
: again immediate ['] branch , here - , ;
: while immediate ['] 0branch , here swap 0 , ;
: repeat immediate ['] branch , here - , dup here swap - swap ! ;

: char word drop c@ ;

: [char] immediate char ['] lit , , ;

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
 
 
: c,
 here c!
 1	 dp +!
;

: cellalign cells 1- + cells 1- invert and ;

: align here cellalign dp ! ;

: '"' [char] " ;

6969696969 .

: s" immediate

  state @ if

	['] litstring ,
	 here
         0 ,
	 begin

	    key
	    dup '"' <>
	 while
	    c,
	 repeat
	 drop
	 0 c, ( include a null terminator )
	 dup
	 here swap -
	 cells - 1- ( don't count the null )
	 swap !
	 align
    else
       here
       begin
           key
	   dup '"' <>
       while
           over c!
	   1+
       repeat
       drop
       here -
       here
       swap
       then
     ;

: ." immediate
  state @ if
    [compile] s"
    ['] tell ,
  else
    begin
      key
      dup '"' = if
        drop
	exit
      then
      emit
    again
  then
  ;


\ : test dup if dup . 1- recurse then 69 . ;


\ : test2 1 if 5 . else 6 . then 7 . ;


\ : test 5 begin dup . 1- 0= until ;

