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


: load-uname \ load uname into here (transiently)
  here
  [c] uname 1 end-c drop
 ;

: get-uname-fld ( entry -- addr )
  here 256 -rot * +
 ;

