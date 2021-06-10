
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


: constant
  word
  header,
  docol ,
  ['] lit ,
  ,
  ['] exit
;

: create
  word header,
  dodoes , 0 ,
;

: does>
    r> latest @ >dfa !
;

: allot
   dp +!
 ;

: variable
  word header,
  dodoes , 0 ,
  1 cells allot
 ;

\ : test dup if dup . 1- recurse then 69 . ;


\ : test2 1 if 5 . else 6 . then 7 . ;


\ : test 5 begin dup . 1- 0= until ;


: clean-stack ( n -- )
  dsp@ swap 1+ cells + dsp!
 ;


: end-c ( args... rv n_args -- rv )
   swap
   >r ( save return value )
   clean-stack
   r> ( restore return value )
 ;


: c-call ( n_m … n_2 n_1 — rv )
   word dlsym c-invoke
   word number end-c
  ;

: c-compile immediate
  ['] lit ,
  word dlsym ,
  ['] c-invoke ,
 ;

: open ( mode addr length -- rv )
  drop
  c-compile open 2 end-c
 ;

: opendir ( addr length -- dirp )
  drop
  c-compile opendir 1 end-c
 ;

: readdir ( dirp -- dirp dirent )
   dup
   c-compile readdir 1 end-c
 ;



: puts ( c-str -- )
  c-compile puts 1 end-c drop
 ;

: displaydir ( dir -- dir )
   begin
     readdir
     dup \ nul entry?

   while
     8 + puts
   repeat

   drop
 ;


: prep-hash-data
   here rot swap 
 ;

: md5 ( addr len -- md5_addr md5_len ) 
  prep-hash-data
  c-compile CC_MD5 3 end-c \ leaves buffer on stack
  16	 \ add length (16 by definition)
 ;

: sha1 
  prep-hash-data
  c-compile CC_SHA1 3 end-c
  20	\ add length (20 by definition)
;

: sysctl ( newdatalen newdata olddatalen olddata mib-length mib -- rv )
  c-compile sysctl 6 end-c
 ;


: CTL_KERN 1 ;

: KERN_OSRELEASE 2 ;

\ 0x200000001 dsp@ >r 0 0 here 0 2 r> sysctl

: get-darwin-ver
 0x200000001
 dsp@ >r 0 0 here 0 2 r> sysctl drop 
 dsp@ >r 0 0 here here 1 cells + 2 r> sysctl drop
 drop
 here 1 cells +  here @
 ;

: load-uname \ load uname into here (transiently)
  here
  c-compile uname 1 end-c drop
 ;

: get-uname-fld ( entry -- addr )
  here 256 -rot * +
 ;

: getifaddrs
   0 dsp@ 
   c-compile getifaddrs 1 end-c drop
 ;

: step-ifaddr
  @
 ;

: get-ifname
   dup if
     dup 1 cells + @ 
   then
 ;


: ' word (find) >cfa ;

: xml-wrap ( tag-addr tag-len xt -- )
  >r 2dup ." <" tell ." >" r>
  execute
  ." </" tell ." >"
 ;


 
