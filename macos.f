\ : test dup if dup . 1- recurse then 69 . ;


\ : test2 1 if 5 . else 6 . then 7 . ;


\ : test 5 begin dup . 1- 0= until ;




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

\ 0 s" testing 123" swap 3 c-call send 4
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


