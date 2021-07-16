  
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

: dlsym ( sym-name sym-len -- sym-addr )
  drop -2 dlsym-addr c-invoke 2 end-c
 ;
 


: c-call ( n_m … n_2 n_1 — rv )
   word dlsym c-invoke
   word number end-c
  ;

: [c] immediate
  ['] lit ,
  word dlsym ,
  ['] c-invoke ,
 ;

: open ( mode addr length -- rv )
  drop
  [c] open 2 end-c
 ;

: opendir ( addr length -- dirp )
  drop
    [c] opendir 1 end-c
 ;

: readdir ( dirp -- dirp dirent )
   dup
   [c] readdir 1 end-c
 ;



: puts ( c-str -- )
  [c] puts 1 end-c drop
 ;

: displaydir ( dir -- dir )
   begin
     readdir
     dup \ nul entry?

   while
     8 + dup [c] strlen 1 end-c tell cr
   repeat

   drop
 ;


: prep-hash-data
   here rot swap 
 ;

: md5 ( addr len -- md5_addr md5_len ) 
  prep-hash-data
  [c] CC_MD5 3 end-c \ leaves buffer on stack
  16	 \ add length (16 by definition)
 ;

: sha1 
  prep-hash-data
  [c] CC_SHA1 3 end-c
  20	\ add length (20 by definition)
;

: sysctl-raw ( newdatalen newdata olddatalen olddata mib-length mib -- rv )
  [c] sysctl 6 end-c
 ;


: CTL_KERN 1 ;

: KERN_OSRELEASE 2 ;

\ 0x200000001 dsp@ >r 0 0 here 0 2 r> sysctl

: get-darwin-ver ( -- addr len )
 0x200000001
 dsp@ >r 0 0 here 0 2 r> sysctl-raw drop 
 dsp@ >r 0 0 here here 1 cells + 2 r> sysctl-raw drop
 drop
 here 1 cells +  here @ tell
 ;


: sysctl ( ctl var -- data len )
  32 shl or
  0 here !
  dsp@ >r 0 0 here 0 2 r> sysctl-raw drop
  dsp@ >r 0 0 here here 1 cells + 2 r> sysctl-raw drop
  drop
  here 1 cells + here @
 ;


: load-uname \ load uname into here (transiently)
  here
  [c] uname 1 end-c drop
 ;

: get-uname-fld ( entry -- addr )
  here 256 -rot * +
 ;

: getifaddrs
   0 dsp@ 
   [c] getifaddrs 1 end-c drop
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
  >r 2dup ." <" tell ." >" r> cr
  execute cr

  ." </" tell ." >" cr
 ;


 
\ 8 2 or s" /System/Library/Frameworks/Security.framework/Security" drop c-call dlopen 2
\

\ dup s" SSLNewContext" drop swap c-call dlsym 2 


: get-time
  0 [c] time 1 end-c .
 ;

: get-hostname
  load-uname
  1 get-uname-fld 256 tell
 ;

: get-platform
  platform tell
 ;

: get-arch
   arch tell
 ;

: get-etc-dir
  s" /etc/" opendir
  dup displaydir
  [c] closedir 1 end-c
  drop drop
 ;


: hw-uuid ( -- uuid2 uuid1 )
 0 0 dsp@ >r
 0 0 dsp@ r>
 [c] gethostuuid 2 end-c
 drop drop drop
; 

: get-hw-uuid
  hw-uuid . .
;


: xmlify immediate
  word (find) dup word-name
  state @ if
   swap
   ['] lit , ,
   ['] lit , ,
   ['] lit , >cfa ,
   ['] xml-wrap ,
  else
   -rot >cfa xml-wrap
  then
; 

: binary-info
   xmlify get-arch
   xmlify get-platform
; 

: posix-info
   xmlify get-time
   xmlify get-hostname
; 

: get-hw-product
  6 27 sysctl tell
 ; 

: darwin-info
   xmlify get-darwin-ver
   xmlify get-etc-dir
   xmlify get-hw-uuid
   xmlify get-hw-product
 ;

: request
  xmlify binary-info 
  xmlify posix-info
  xmlify darwin-info
 ;

\ request

\ 8 2 or s" /System/Library/Frameworks/IOKit.framework/IOKit" drop c-call dlopen 2

\ dup s" kIOMasterPortDefault" drop swap c-call dlsym 2 .

8 2 or s" /usr/lib/libcrypto.35.dylib" drop c-call dlopen 2

\ c-call BIO_s_mem 0 c-call BIO_new 1

s" 127.0.0.1" drop c-call inet_addr 1
0 here c! 2 here 1 + c! 0x55 here 2 + c! 0x55 here 3 + c!
here 4 + !
0 here 8 + !
0 1 2 c-call socket 3
dup >r 16 here r> c-call connect 3 drop

\ 0 s" test" swap 3 c-call send 4
\ 0 128 here 3 c-call recv 4 



: create-4th-ctx ( xt-ctx xt -- <forth-state-ptr> )
   forth-state@ dsp@
 ;


: blah ( n -- funny-num ) . 0x69 ;

\  0 ' get-darwin-ver create-4th-ctx invoke-forth c-invoke
\ 0 dsp@ >r  0x55 ' blah forth-state@ dsp@ invoke-forth 0 r> c-call pthread_create 4

\ 0x000070000612a928


\ todo: create-forth-state ( create a forth state with new dp )
\ todo: use pointer tagging for word flags



\ objc stuff
 8 2 or s" /System/Library/Frameworks/Foundation.framework/Foundation" drop c-call dlopen 2 drop
\ s" hello" drop
\ s" stringWithUTF8String:" drop c-call sel_registerName 1
\ s" NSString" drop c-call objc_getClass 1
\ c-call objc_msgSend 3




: objc ( args obj -- rv )
  word drop [c] sel_registerName 1 end-c
  swap
  [c] objc_msgSend 2 end-c
  >r  word number clean-stack r>
 ; 

: objc-static ( args -- rv)
   word drop [c] sel_registerName 1 end-c
   word drop [c] objc_getClass 1 end-c
   [c] objc_msgSend 2 end-c
   >r  word number clean-stack r>
 ;

: nsstring ( addr len -- nsstring )
   drop s" stringWithUTF8String:" drop [c] sel_registerName 1 end-c
   s" NSString" drop [c] objc_getClass 1 end-c
   [c] objc_msgSend 3 end-c
 ;


s" test" drop objc-static stringWithUTF8String: NSString 1


