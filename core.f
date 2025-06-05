  
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
: cr
  10 emit
 ;

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


: cells cellsize * ;

	
 : nip ( x y -- y ) swap drop ;
 : tuck ( x y -- y x y ) dup rot ;
 : pick 1+ cellsize * dsp@ + @ ;

: space 32 emit ;

: .s '(' emit space
	      dsp@
	      0 s0 + 1 cells -
	      begin
	      over over <=
	      while
	      dup @ .
	      space
              1 cells -
	      repeat
	      drop drop
	      ')' emit space
 ;
 
 
: c,
 here c!
 1 dp +!
;

: cellalign 1 cells 1- + 1 cells 1- invert and ;

: align here cellalign dp ! ;

: '"' [char] " ;

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
         1 cells - 1- ( don't count the null )
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
       0 over c! 1+ \ add NUL byte

       
       here - 1- \ don't include NUL byte in length
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


: :noname
  0 0 header,
  here docol ,
  ]
;

: constant
  word
  header,
  docol ,
  ['] lit ,
  ,
  ['] exit ,
;

: create
  word header,
  dodoes , 0 ,
;

: does>
    r> latest @ >dfa !
;

: word-len ( word-addr -- name-len )
   1 cells + @
   0xff and
   3 shr
 ;


: word-name ( word-addr -- name-addr )
   1 cells + dup    \ prep stack
   @ 0xff and 3 shr \ calc size
   swap 1+          \ adjust addr
   swap             \ setup for string
;

: allot
   dp +!
 ;

: variable
  word header,
  dodoes , 0 ,
  1 cells allot
 ;

