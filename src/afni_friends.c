#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <cs.h>

/*-------------------------------------------------------------------------*/
/*! What we might thank people for. */

static char * afni_helptypes[] = {
   "advice and help"                ,  /* mask =   1 */
   "much encouragement"             ,  /* mask =   2 */
   "many suggestions"               ,  /* mask =   4 */
   "useful feedback"                ,  /* mask =   8 */
   "\"quick\" questions"            ,  /* mask =  16 */
   "inspiration"                    ,  /* mask =  32 */
   "great efforts"                  ,  /* mask =  64 */
   "caloric input"                  ,  /* mask = 128 */
   "awe-inspiring beer consumption" ,  /* mask = 256 */
   "awe-inspiring caffeine binging" ,  /* mask = 512 */
   "you-know-what"                     /* mask =1024 */
} ;

/*-- special codes --*/

#define YOU_KNOW_WHAT 10
#define INSPIRATION    5
#define KLOSEK         0   /* person index below */

#define NUM_HELPTYPES (sizeof(afni_helptypes)/sizeof(char *))

typedef struct { char *name ; int helpmask ; } AFNI_friend ;

/*! Who we might thank. */

static AFNI_friend afni_friends[] = {
  { "MM Klosek"      , ( 1 | 2              | 32                 | 1024 ) } ,
  { "JR Binder"      , ( 1 |     4 | 8 | 16                             ) } ,
  { "EA DeYoe"       , ( 1 |     4 | 8                                  ) } ,
  { "JS Hyde"        , ( 1 | 2              | 32                        ) } ,
  { "SM Rao"         , ( 1 |     4 | 8 | 16           | 128             ) } ,
  { "EA Stein"       , ( 1 | 2 | 4 | 8 | 16           | 128             ) } ,
  { "A Jesmanowicz"  , (             8 |      32                        ) } ,
  { "MS Beauchamp"   , ( 1 | 2 | 4 | 8 | 16 | 32      | 128             ) } ,
  { "JA Bobholz"     , (             8 | 16 | 32      | 128             ) } ,
  { "JA Frost"       , (             8 | 16           | 128             ) } ,
  { "J Kummer"       , (         4 | 8      | 32                        ) } ,
  { "BD Ward"        , (         4 | 8           | 64       | 512       ) } ,
  { "S Marrett"      , (             8 | 16                             ) } ,
  { "T Holroyd"      , (             8 | 16                             ) } ,
  { "KM Donahue"     , (             8 | 16                             ) } ,
  { "PA Bandettini"  , (                 16                 | 512       ) } ,
  { "AS Bloom"       , ( 1 | 2         | 16                             ) } ,
  { "T Ross"         , (         4 | 8 | 16 | 32                        ) } ,
  { "H Garavan"      , (         4 | 8 | 16                 | 256       ) } ,
  { "SJ Li"          , (     2                                          ) } ,
  { "M Huerta"       , (     2                                          ) } ,
  { "ZS Saad"        , (     2 | 4 | 8 | 16      | 64 | 128             ) } ,
  { "K Ropella"      , (     2                                          ) } ,
  { "B Knutson"      , (                 16 |           128             ) } ,
  { "B Biswal"       , (                 16                             ) } ,
  { "RM Birn"        , (             8 | 16 |           128 | 512       ) } ,
  { "V Roopchansingh", (         4 | 8 | 16      | 64                   ) } ,
  { "J Ratke"        , (                 16                             ) } ,
  { "PSF Bellgowan"  , (             8 | 16                             ) } ,
  { "S Durgerian"    , (             8 | 16                             ) } ,
  { "M Belmonte"     , (             8 |           64                   ) } ,
  { "K Bove-Bettis"  , (             8 | 16 |           128             ) } ,
  { "E Kapler"       , (                                128             ) } ,
  { "R Doucette"     , (                           64 | 128             ) } ,
  { "K Kuhns"        , (                           64 | 128             ) } ,
  { "RC Reynolds"    , (                           64 | 128 | 512       ) } ,
  { "PP Christidis"  , (                           64 | 128 | 512       ) } ,
  { "G Fong"         , (                 16 |           128             ) } ,
  { "LR Frank"       , (             8 | 16                             ) } ,
  { "R Desimone"     , (     2                                          ) } ,
  { "S Amara"        , ( 1 | 2                                          ) } ,
  { "L Ungerleider"  , (     2                                          ) } ,
  { "KR Hammett"     , (             8 |           64                   ) } ,
  { "A Clark"        , (                           64 |       512       ) } ,
  { "J Rajendra"     , (                           64 | 128 | 512       ) } ,
  { "B Pittman"      , (                           64 |       512       ) } ,
  { "DS Cohen"       , ( 1 | 2                                          ) } ,
  { "DA Jacobson"    , ( 1 | 2              | 32                        ) } ,
  { "DR Glen"        , (         4 | 8 |           64                   ) } ,
  { "G Chen"         , (         4 | 8 |           64                   ) } ,
  { "M Furey"        , (             8 |                128             ) } ,
  { "J Bodurka"      , (         4 | 8 |                      256       ) } ,
  { "JA Butman"      , (     2 | 4 | 8                                  ) } ,
  { "E Ricciardi"    , (     2 |                              512       ) } ,
  { "N Vanello"      , (     2 |                              512       ) } ,
  { "B Feige"        , ( 1 |         8                                  ) } ,
  { "K Murphy"       , (             8 | 16                 | 256       ) } ,
  { "A Martin"       , (     2                                          ) } ,
  { "Marieke Mur"    , (                                128             ) } ,
  { "J Haxby"        , (     2                                          ) } ,
  { "I Gobbini"      , (     2                                          ) } ,
  { "SM Laconte"     , (                      32 | 64                   ) } ,
  { "L Pezawas"      , (     2 | 4 | 8                                  ) } ,
  { "W Gaggl"        , (         4 | 8                                  ) } ,
  { "V Hradil"       , (         4 | 8 | 16                             ) } ,
  { "D Handwerker"   , (         4 | 8                                  ) } ,
  { "J Gonzalez-Castillo" , (    4 | 8                | 128             ) } , /* name is too long :( */
  { "J Evans"        ,      (    4 | 8                | 128             ) } ,
  { "S Inati"        ,      (    4 | 8                                  ) } ,
  { "P Taylor"       ,      (                      64 | 128 | 512       ) } ,
  { "P Kundu"        ,      (                      64                   ) } ,
  { "S Gotts"        ,      (    8 |          32                        ) } ,
  { "S Japee"        ,      (                           128             ) } ,
  { "X Weng"         ,      (    8                                      ) } ,
  { "P Molfese"      ,      (    8 | 16 |               128             ) } ,
  { "E Leibenluft"   ,      ( 2                                         ) } ,
  { "D Pine"         ,      ( 2                                         ) } ,
  { "P Kohn"         ,      ( 2 | 8                                     ) } ,
  { "C Baker"        ,      ( 2                                         ) }
} ;

#define NUM_FRIENDS (sizeof(afni_friends)/sizeof(AFNI_friend))

/*---------------------------------------------------------------------*/
/*! Return a "thanks" string (static storage - don't free it). */

char * AFNI_get_friend(void)
{
   static char buf[256] ; int nf , nh , hmask , qq=0 ;
   nf = lrand48() % NUM_FRIENDS ;
   do{
     nh = lrand48() % NUM_HELPTYPES ; hmask = 1 << nh ; qq++ ;
   } while( qq < 73 && (hmask & afni_friends[nf].helpmask) == 0 ) ;

   if( nh == YOU_KNOW_WHAT && nf != KLOSEK ) nh = INSPIRATION; /* only Gosia */

   sprintf( buf  ,
            "Thanks go to %s for %s" ,
            afni_friends[nf].name , afni_helptypes[nh] ) ;
   return buf ;
}

/*------------------------------------------------------------------------------*/
/* 25 Nov 2002: this date in history! */

#define JAN  1
#define FEB  2
#define MAR  3
#define APR  4
#define MAY  5
#define JUN  6
#define JUL  7
#define AUG  8
#define SEP  9
#define OCT 10
#define NOV 11
#define DEC 12

/*! The struct storing date trivia. */

typedef struct { int mon,day; char *label; } mday ;

/*-----------------------------------------*/
/*! max # trivia elements allowed per date */

#define NTMAX 99

char * AFNI_get_date_trivia(void) ;

static int  ntar=0 ;
static char *tar[NTMAX] ;

int AFNI_get_todays_trivia( char ***triv )
{
   (void)AFNI_get_date_trivia() ;
   if( triv != NULL ) *triv = tar ;
   return ntar ;
}
/*-----------------------------------------*/

/*! The date trivia array. */

static mday holiday[] = {
   {JAN, 1,"Anniversary of Emancipation Proclamation"                } ,  /* WW */
   {JAN, 1,"New Year's Day"                                          } ,
   {JAN, 1,"Lorenzo de Medici's birthday"                            } ,
   {JAN, 1,"Paul Revere's birthday"                                  } ,
   {JAN, 1,"Cameroon Independence Day"                               } ,
   {JAN, 1,"Haiti Independence Day"                                  } ,
   {JAN, 1,"Western Samoa Independence Day"                          } ,
   {JAN, 1,"Sudan Independence Day"                                  } ,
   {JAN, 1,"Satyendra Nath Bose's birthday"                          } ,
   {JAN, 2,"Isaac Asimov's birthday"                                 } ,
   {JAN, 3,"John Ronald Reuel Tolkien's birthday"                    } ,
   {JAN, 3,"Anniversary of Battle of Princeton"                      } ,
   {JAN, 4,"Burma Independence Day"                                  } ,
   {JAN, 5,"King Juan Carlos I's birthday"                           } ,
   {JAN, 5,"Konrad Adenauer's birthday"                              } ,
   {JAN, 6,"Sherlock Holmes' birthday"                               } ,
   {JAN, 6,"Birthday of Mr Spock"                                    } ,
   {JAN, 6,"Jeanne d'Arc's birthday"                                 } ,
   {JAN, 7,"Millard Fillmore's birthday"                             } ,
   {JAN, 8,"Stephen Hawking's birthday"                              } ,
   {JAN, 8,"Elvis Presley's birthday"                                } ,
   {JAN, 9,"Richard Nixon's birthday"                                } ,
   {JAN,10,"Donald Knuth's birthday"                                 } ,
   {JAN,10,"Caesar crosses the Rubicon"                              } ,
   {JAN,10,"Anniversaire du Marechal Ney"                            } ,
   {JAN,11,"Alexander Hamilton's birthday"                           } ,
   {JAN,11,"Chad Independence Day"                                   } ,
   {JAN,13,"Anniversary of Zola's J'Accuse"                          } ,
   {JAN,14,"Albert Schweitzer's birthday"                            } ,
   {JAN,14,"Alfred Tarski's birthday"                                } ,
   {JAN,14,"Anniversary of Simpson's premiere"                       } ,
   {JAN,15,"Martin Luther King Jr's birthday"                        } ,
   {JAN,15,"Chuck Berry's birthday"                                  } ,
   {JAN,16,"David Lloyd George's birthday"                           } ,
   {JAN,16,"Anniversary of Prohibition's start"                      } ,
   {JAN,17,"Benjamin Franklin's birthday"                            } ,
   {JAN,17,"Muhammad Ali's birthday"                                 } ,
   {JAN,17,"James Earl Jones' birthday"                              } ,
   {JAN,17,"Latvia's Zirgu Diena"                                    } ,
   {JAN,18,"Cary Grants' birthday"                                   } ,
   {JAN,18,"Saint Malgorzata's (of Hungary) Day"                     } ,
   {JAN,18,"Daniel Webster's birthday"                               } ,
   {JAN,19,"Edgar Allen Poe's birthday"                              } ,
   {JAN,20,"Anniversary of end of American Revolution"               } ,
   {JAN,20,"George Burns' birthday"                                  } ,
   {JAN,20,"Penguin Awareness Day"                                   } ,
   {JAN,21,"Squirrel Appreciation Day"                               } ,
   {JAN,23,"David Hilbert's birthday"                                } ,
   {JAN,23,"Humphrey Bogart's birthday"                              } ,
   {JAN,24,"Anniversary of beer in cans"                             } ,
   {JAN,24,"Emperor Hadrian's birthday"                              } ,
   {JAN,25,"Somerset Maugham's birthday"                             } ,
   {JAN,26,"Anniversary of First Fleet to Botany Bay"                } ,
   {JAN,26,"Australia Day"                                           } ,
   {JAN,27,"Charles Dodgson's (Lewis Carroll) birthday"              } ,
   {JAN,27,"Wolfgang Amadeus Mozart's birthday"                      } ,
   {JAN,28,"Anniversary of Challenger explosion"                     } ,
   {JAN,29,"WC Fields' birthday"                                     } ,
   {JAN,30,"Franklin Delano Roosevelt's birthday"                    } ,
   {JAN,31,"Queen Beatrix's birthday"                                } ,
   {JAN,31,"Jackie Robinson's birthday"                              } ,
   {JAN,31,"Nauru Independence Day"                                  } ,

   {FEB, 1,"Clark Gable's birthday"                                  } , /* WW */
   {FEB, 2,"Charles Maurice de Talleyrand's birthday"                } ,
   {FEB, 2,"Tom Smothers' birthday"                                  } ,
   {FEB, 2,"Groundhog Day"                                           } ,
   {FEB, 3,"The Day the Music Died"                                  } ,
   {FEB, 4,"Charles Lindbergh's birthday"                            } ,
   {FEB, 4,"Sri Lanka Independence Day"                              } ,
   {FEB, 4,"Tadeusz Kosciusko's birthday"                            } ,
   {FEB, 4,"Charles Lindbergh's birthday"                            } ,
   {FEB, 5,"Hank Aaron's birthday"                                   } ,
   {FEB, 6,"Babe Ruth's birthday"                                    } ,
   {FEB, 6,"Ronald Reagan's birthday"                                } ,
   {FEB, 6,"New Zealand Day"                                         } ,
   {FEB, 7,"Thomas More's birthday"                                  } ,
   {FEB, 7,"Grenada Independence Day"                                } ,
   {FEB, 7,"Charles Dickens' birthday"                               } ,
   {FEB, 7,"Dmitri Mendeleev's birthday"                             } ,
   {FEB, 8,"William Tecumseh Sherman's birthday"                     } ,
   {FEB, 8,"Jules Verne's birthday"                                  } ,
   {FEB,10,"St. Scholastica's day"                                   } ,
   {FEB,10,"Anniversary of Treaty of Paris, 1763"                    } ,
   {FEB,11,"Josiah Willard Gibb's birthday"                          } ,
   {FEB,11,"Seoul (Hanyang U) Bootcamp Anniversary"                  } ,
   {FEB,11,"Thomas Edison's birthday"                                } ,
   {FEB,11,"Iran National Day"                                       } ,
   {FEB,12,"Abraham Lincoln's birthday"                              } ,
   {FEB,12,"Charles Darwin's birthday"                               } ,
   {FEB,13,"Kim Novak's birthday"                                    } ,
   {FEB,13,"The Ides of February"                                    } ,
   {FEB,14,"Saint Valentine's Day"                                   } ,
   {FEB,15,"Galileo Galilei's birthday"                              } ,
   {FEB,18,"Gambia Independence Day"                                 } ,
   {FEB,19,"Nikolaus Kopernikus' birthday"                           } ,
   {FEB,20,"Anniversary of John Glenn's spaceflight"                 } ,
   {FEB,21,"Anniversary of start of the Battle of Verdun"            } ,
   {FEB,22,"George Washington's birthday"                            } ,
   {FEB,22,"Frederic Chopin's birthday"                              } ,
   {FEB,22,"Saint Malgorzata's (of Cortona) Day"                     } ,
   {FEB,24,"St. Ethelbert's day"                                     } ,
   {FEB,25,"Auguste Renoir's birthday"                               } ,
   {FEB,25,"Kuwait National Day"                                     } ,
   {FEB,25,"Anniversary of Battle of Los Angeles"                    } ,
   {FEB,27,"Dominican Republic Independence Day"                     } ,
   {FEB,27,"Anniversary of Lincoln's Cooper Union Speech"            } ,
   {FEB,27,"John Steinbeck's birthday"                               } ,
   {FEB,28,"Linus Pauling's birthday"                                } ,
   {FEB,29,"Herman Hollerith's birthday"                             } ,
   {FEB,29,"W E B Dubois' birthday"                                  } ,

   {MAR, 1,"Santiago Ramon y Cajal's birthday"                       } ,
   {MAR, 1,"National Pig Day"                                        } ,
   {MAR, 2,"Samuel Houston's birthday"                               } ,
   {MAR, 3,"Georg Cantor's birthday"                                 } ,
   {MAR, 3,"Morocco National Day"                                    } ,
   {MAR, 4,"Casimir Pulaski's birthday"                              } ,
   {MAR, 5,"Anniversary of Boston Massacre"                          } ,
   {MAR, 6,"Michelangelo Buonarroti's birthday"                      } ,
   {MAR, 7,"Anniversary of Okazaki Bootcamp"                         } ,
   {MAR, 8,"Kenneth Grahame's birthday"                              } ,
   {MAR, 8,"International Women's Day"                               } ,
   {MAR,11,"World Day of Muslim Culture"                             } ,
   {MAR,11,"Douglas Adams' birthday"                                 } ,
   {MAR,12,"Kemal Ataturk's birthday"                                } ,
   {MAR,12,"Mauritius Independence Day"                              } ,
   {MAR,14,"Albert Einstein's birthday"                              } ,
   {MAR,15,"The Ides of March - Beware!"                             } ,
   {MAR,16,"James Madison's birthday"                                } ,
   {MAR,17,"Saint Patrick's Day"                                     } ,
   {MAR,17,"Anniversario dell'Unita d'Italia!"                       } ,
   {MAR,18,"Aruba National Day"                                      } ,
   {MAR,19,"Saint Joseph's Day"                                      } ,
   {MAR,20,"Anniversary of Uncle Tom's Cabin"                        } ,
   {MAR,20,"International Day of Happiness"                          } ,
   {MAR,20,"Extraterrestrial Abduction Day!"                         } ,
   {MAR,20,"World Frog Day"                                          } ,
   {MAR,21,"Jean Baptiste Joseph Fourier's birthday"                 } ,
   {MAR,21,"World Poetry Day"                                        } ,
   {MAR,23,"Emmy Noether's birthday"                                 } ,
   {MAR,23,"Pakistan Day"                                            } ,
   {MAR,24,"Anniversary of Kosciusko's Oath to Nation"               } ,
   {MAR,25,"Anniversary of Downfall of Sauron"                       } ,
   {MAR,25,"Greece Independence Day"                                 } ,
   {MAR,25,"Old English New Year's Day"                              } ,
   {MAR,26,"Bangladesh Independence Day"                             } ,
   {MAR,26,"Robert Frost's birthday"                                 } ,
   {MAR,28,"Pierre Simon de Laplace's birthday"                      } ,
   {MAR,30,"Vincent van Gogh's birthday"                             } ,
   {MAR,31,"Rene Descartes' birthday"                                } ,

   {APR, 1,"April Fool's Day"                                        } ,
   {APR, 2,"Charlemagne's birthday"                                  } ,
   {APR, 2,"National Ferret Day"                                     } ,
   {APR, 3,"Stanislaw Ulam's birthday"                               } ,
   {APR, 4,"Anniversary of founding of NATO"                         } ,
   {APR, 4,"Hungary Liberation Day"                                  } ,
   {APR, 4,"Senegal Independence Day"                                } ,
   {APR, 4,"Yamamoto Isoroku's birthday"                             } ,
   {APR, 6,"Birthday of the Twinkie"                                 } ,
   {APR, 8,"Siddhartha Gautama's birthday"                           } ,
   {APR, 9,"Anniversary of Lee's surrender at Appomattox"            } ,
   {APR,10,"Anniversary of loss of USS Thresher"                     } ,
   {APR,10,"National Sibling Day (USA)"                              } ,
   {APR,11,"Dean Acheson's birthday"                                 } ,
   {APR,11,"Tulsa Bootcamp Anniversary"                              } ,
   {APR,12,"Anniversary of Yuri Gagarin's spaceflight"               } ,
   {APR,13,"Thomas Jefferson's birthday"                             } ,
   {APR,13,"New Year's Day (SE Asia)"                                } ,
   {APR,14,"Anniversary of Lincoln's assasination :("                } ,
   {APR,14,"National Dolphin Day"                                    } ,
   {APR,15,"Anniversary of sinking of Titanic"                       } ,
   {APR,15,"Arnold Toynbee's birthday"                               } ,
   {APR,15,"Leonhard Euler's birthday"                               } ,
   {APR,16,"Jerzy Neyman's birthday"                                 } ,
   {APR,17,"Syria Independence Day"                                  } ,
   {APR,18,"Anniversary of Tulsa Bootcamp II"                        } ,
   {APR,18,"Anniversary of Paul Revere's Ride"                       } ,
   {APR,19,"Anniversary of Warsaw Ghetto Uprising"                   } ,
   {APR,19,"Chongqing SWU Bootcamp Anniversary"                      } ,
   {APR,21,"Queen Elizabeth II's birthday"                           } ,
   {APR,21,"Birthday of Marcus Aurelius"                             } ,
   {APR,21,"Birthday of Napoleon III"                                } ,
   {APR,21,"Anniversary of the founding of Rome"                     } ,
   {APR,22,"Earth Day"                                               } ,
   {APR,22,"Queen Isabella's birthday"                               } ,
   {APR,22,"Sir Michael Atiyah's birthday"                           } ,
   {APR,23,"Saint George's Day"                                      } ,
   {APR,24,"Henri Philippe Petain's birthday"                        } ,
   {APR,25,"Oliver Cromwell's birthday"                              } ,
   {APR,25,"Ross Lockridge's birthday"                               } ,
   {APR,25,"Felix Klein's birthday"                                  } ,
   {APR,26,"William Shakespeare's birthday"                          } ,
   {APR,26,"Ludwig Wittgenstein's birthday"                          } ,
   {APR,26,"David Hume's birthday"                                   } ,
   {APR,27,"Ulysses Grant's birthday"                                } ,
   {APR,27,"Sierra Leone Independence Day"                           } ,
   {APR,27,"Togo Independence Day"                                   } ,
   {APR,28,"Kurt Goedel's birthday"                                  } ,
   {APR,28,"Sir Terry Pratchett's birthday"                          } ,
   {APR,28,"Bahai Feast of Jamal"                                    } ,
   {APR,29,"Duke Ellington's birthday"                               } ,
   {APR,29,"Henri Poincare's birthday"                               } ,
   {APR,30,"Karl Friedrich Gauss's birthday"                         } ,
   {APR,30,"Walpurgisnacht"                                          } ,

   {MAY, 1,"Emperor Claudius's birthday"                             } ,
   {MAY, 1,"Herman Melville's birthday"                              } ,
   {MAY, 1,"Anniversary of Crab Nebula Supernova"                    } ,
   {MAY, 1,"May Day"                                                 } ,
   {MAY, 1,"Bealtaine"                                               } ,
   {MAY, 2,"Baron Von Richtofen's birthday"                          } ,
   {MAY, 3,"Anniversary of Polish Constitution"                      } ,
   {MAY, 3,"Nicolo Machiavelli's birthday"                           } ,
   {MAY, 3,"Golda Meir's birthday"                                   } ,
   {MAY, 4,"Audrey Hepburn's birthday"                               } ,
   {MAY, 4,"Star Wars Day"                                           } ,
   {MAY, 5,"Cinco de Mayo"                                           } ,
   {MAY, 5,"Henryk Sienkiewicz's birthday"                           } ,
   {MAY, 6,"Anniversary of Hindenburg explosion"                     } ,
   {MAY, 7,"Anniversary of sinking of Lusitania"                     } ,
   {MAY, 7,"Anniversary of HMS Victory launch"                       } ,
   {MAY, 8,"VE Day"                                                  } ,
   {MAY, 8,"Harry S Truman's birthday"                               } ,
   {MAY, 9,"John Brown's birthday"                                   } ,
   {MAY, 9,"Anton Cermak's birthday"                                 } ,
   {MAY, 9,"Europe Day"                                              } ,
   {MAY,10,"Gustav Stresemann's birthday"                            } ,
   {MAY,10,"Fred Astaire's birthday"                                 } ,
   {MAY,11,"Laos Constitution Day"                                   } ,
   {MAY,11,"Richard Feynman's birthday"                              } ,
   {MAY,12,"Florence Nightingale's birthday"                         } ,
   {MAY,13,"Anniversary of Jamestown settlement"                     } ,
   {MAY,14,"Paraguay Independence Day"                               } ,
   {MAY,15,"Israel Independence Day"                                 } ,
   {MAY,15,"The Ides of May"                                         } ,
   {MAY,16,"Maria Gaetana Agnesi's birthday"                         } ,
   {MAY,16,"Try Not To Be Too Stupid Day"                            } ,
   {MAY,16,"A Day of Unforgettable Exaltation"                       } ,
   {MAY,17,"Norway National Day"                                     } ,
   {MAY,18,"John Paul II's birthday"                                 } ,
   {MAY,18,"Bertrand Russell's birthday"                             } ,
   {MAY,19,"Malcom X's birthday"                                     } ,
   {MAY,20,"John Stuart Mill's birthday"                             } ,
   {MAY,20,"Cuba Independence Day"                                   } ,
   {MAY,21,"Andrei Sakharov's birthday"                              } ,
   {MAY,22,"Arthur Conan Doyle's birthday"                           } ,
   {MAY,23,"John Bardeen's birthday"                                 } ,
   {MAY,23,"World Turtle Day"                                        } ,
   {MAY,24,"Oliver Cromwell's birthday"                              } ,
   {MAY,25,"Argentina Revolution Day"                                } ,
   {MAY,25,"Jordan Independence Day"                                 } ,
   {MAY,26,"Wear a Towel Day"                                        } ,
   {MAY,26,"Guyana Independence Day"                                 } ,
   {MAY,26,"Founding of US NIH (1930)"                               } ,
   {MAY,27,"Wild Bill Hickock's birthday"                            } ,
   {MAY,28,"Ian Fleming's birthday"                                  } ,
   {MAY,29,"John F Kennedy's birthday"                               } ,
   {MAY,29,"Harry Bateman's birthday"                                } ,
   {MAY,29,"Anniversary of first Everest summit"                     } ,
   {MAY,30,"St. Jeanne d'Arc's day"                                  } ,
   {MAY,31,"Walt Whitman's birthday"                                 } ,
   {MAY,31,"South Africa Republic Day"                               } ,
   {MAY,31,"Pisa Bootcamp Anniversary"                               } ,

   {JUN, 1,"Marilyn Monroe's birthday"                               } ,
   {JUN, 1,"Tunisia National Day"                                    } ,
   {JUN, 1,"Anniversary of first of Scotch whisky"                   } ,
   {JUN, 2,"Festa della Repubblica!"                                 } ,
   {JUN, 3,"Birthday of Tony Curtis"                                 } ,
   {JUN, 4,"Anniversary of Battle of Midway"                         } ,
   {JUN, 4,"Hug Your Cat Day"                                        } ,
   {JUN, 5,"John Maynard Keynes' birthday"                           } ,
   {JUN, 5,"Seychelles Independence Day"                             } ,
   {JUN, 5,"Anniversary of Montgolfier Balloon"                      } ,
   {JUN, 6,"D-Day"                                                   } ,
   {JUN, 6,"Pisa Bootcamp Anniversary"                               } ,
   {JUN, 7,"Imre Nagy's birthday"                                    } ,
   {JUN, 8,"Frank Lloyd Wright's birthday"                           } ,
   {JUN, 9,"Anniversary of Donald Duck's debut"                      } ,
   {JUN,10,"Portugal National Day"                                   } ,
   {JUN,10,"White Rose Day"                                          } ,
   {JUN,11,"Ben Jonson's birthday"                                   } ,
   {JUN,11,"John Constable's birthday"                               } ,
   {JUN,12,"Philippines Independence Day"                            } ,
   {JUN,13,"Winfield Scott's birthday"                               } ,
   {JUN,13,"Anniversary of Edict of Milan"                           } ,
   {JUN,13,"William Butler Yeats' birthday"                          } ,
   {JUN,14,"Harriet Beecher Stowe's birthday"                        } ,
   {JUN,14,"Founding of US Army (1775)"                              } ,
   {JUN,14,"Alois Alzheimer's birthday"                              } ,
   {JUN,14,"National Bourbon Day"                                    } ,
   {JUN,15,"Anniversary of Magna Carta"                              } ,
   {JUN,15,"Anniversary of US-British 'Pig War'"                     } ,
   {JUN,16,"Geronimo's birthday"                                     } ,
   {JUN,17,"Anniversary of Battle of Bunker's Hill"                  } ,
   {JUN,17,"Iceland Republic Day"                                    } ,
   {JUN,19,"Juneteenth"                                              } ,
   {JUN,20,"Anniversary of Oxford University"                        } ,
   {JUN,21,"Alexander the Great's birthday"                          } ,
   {JUN,22,"Anniversary of saxophone invention"                      } ,
   {JUN,23,"Alan Turing's birthday"                                  } ,
   {JUN,23,"Luxembourg Grand Duke Day"                               } ,
   {JUN,25,"Anniversary of Battle of the Little Big Horn"            } ,
   {JUN,25,"Mozambique Independence Day"                             } ,
   {JUN,26,"Malagasy Republic Independence Day"                      } ,
   {JUN,27,"Djibouti Independence Day"                               } ,

   {JUL, 1,"Canada Day -- O Canada!"                                 } ,  /* WW */
   {JUL, 1,"Anniversary of Battle of the Somme"                      } ,
   {JUL, 1,"Gottfried Wilhelm Leibniz's birthday"                    } ,
   {JUL, 1,"Burundi Independence Day"                                } ,
   {JUL, 1,"Ghana Republic Day"                                      } ,
   {JUL, 1,"Rwanda Independence Day"                                 } ,
   {JUL, 1,"Somalia Independence Day"                                } ,
   {JUL, 2,"Anniversary of American Independence"                    } ,
   {JUL, 2,"Thurgood Marshall's birthday"                            } ,
   {JUL, 2,"World UFO Day"                                           } ,
   {JUL, 2,"Hans Bethe's birthday"                                   } ,
   {JUL, 3,"Franz Kafka's birthday"                                  } ,
   {JUL, 4,"Anniversary of Declaration of Independence"              } ,
   {JUL, 4,"Anniversary of Vickburg's surrender"                     } ,
   {JUL, 4,"Rube Goldberg's birthday"                                } ,
   {JUL, 5,"Anniversary of Newton's Principia"                       } ,
   {JUL, 5,"Anniversary of introduction of bikini"                   } ,
   {JUL, 5,"Anniversary of Spam (the 'meat')"                        } ,
   {JUL, 5,"Venezuela Independence Day"                              } ,
   {JUL, 5,"Bill Watterson's birthday"                               } ,
   {JUL, 6,"Malawi Independence Day"                                 } ,
   {JUL, 6,"John Paul Jones' birthday"                               } ,
   {JUL, 7,"Satchel Paige's birthday"                                } ,
   {JUL, 7,"Robert Heinlein's birthday"                              } ,
   {JUL, 7,"Anniversary of Sliced Bread!"                            } ,
   {JUL, 9,"Argentina Independence Day"                              } ,
   {JUL, 9,"Palau Constitution Day"                                  } ,
   {JUL,10,"Bahamas Independence Day"                                } ,
   {JUL,11,"Mongolia Revolution Day"                                 } ,
   {JUL,11,"Birthday of Tenzin Gyatso"                               } ,
   {JUL,12,"Sao Tome & Principe Independence Day"                    } ,
   {JUL,13,"Gaius Julius Caesar's birthday"                          } ,
   {JUL,13,"Jean-Luc Picard's birthday"                              } ,
   {JUL,14,"Bastille Day - Vive la France!"                          } ,
   {JUL,14,"Cow Appreciation Day (Moooo)"                            } ,
   {JUL,14,"Iraq Republic Day"                                       } ,
   {JUL,14,"Gerald Ford's birthday"                                  } ,
   {JUL,15,"Anniversary of Battle of Grunwald"                       } ,
   {JUL,15,"Appreciate a Cow Day"                                    } ,
   {JUL,15,"Rembrandt van Rijn's birthday"                           } ,
   {JUL,16,"Anniversary of 1st atomic explosion"                     } ,
   {JUL,16,"Anniversary of Apollo 11's launch"                       } ,
   {JUL,17,"Anniversary of Disneyland's opening"                     } ,
   {JUL,17,"South Korea Constitution Day"                            } ,
   {JUL,18,"John Glenn's birthday"                                   } ,
   {JUL,18,"Nelson Mandela's birthday"                               } ,
   {JUL,18,"Anniversary of Assault on Ft Wagner"                     } ,
   {JUL,19,"George McGovern's birthday"                              } ,
   {JUL,19,"Edgar Degas' birthday"                                   } ,
   {JUL,20,"Anniversary of Apollo 11 Moon landing"                   } ,
   {JUL,20,"Saint Malgorzata's (of Antioch) Day"                     } ,
   {JUL,20,"Sir Edmund Hillary's birthday"                           } ,
   {JUL,20,"Columbia Independence Day"                               } ,
   {JUL,21,"Belgium Independence Day"                                } ,
   {JUL,21,"Martyr's Day in Bolivia"                                 } ,
   {JUL,22,"Friedrich Bessel's birthday"                             } ,
   {JUL,22,"Gregor Mendel's birthday"                                } ,
   {JUL,23,"Raymond Chandler's birthday"                             } ,
   {JUL,23,"Egypt National Day"                                      } ,
   {JUL,24,"Simon Bolivar's birthday"                                } ,
   {JUL,24,"Anniversary of Apollo 11's return to Earth"              } ,
   {JUL,24,"Amelia Earhart's birthday"                               } ,
   {JUL,25,"Arthur Balfour's birthday"                               } ,
   {JUL,26,"Maldives Independence Day"                               } ,
   {JUL,26,"George Bernard Shaw's birthday"                          } ,
   {JUL,28,"Gerard Manley Hopkin's birthday"                         } ,
   {JUL,28,"Peru Independence Day"                                   } ,
   {JUL,29,"Alexis de Tocqueville's birthday"                        } ,
   {JUL,29,"Sigmund Romberg's birthday"                              } ,
   {JUL,30,"Anniversary of Amistad uprising"                         } ,
   {JUL,30,"Vanuatu Independence Day"                                } ,
   {JUL,30,"Arnold Schwarzenegger's birthday"                        } ,
   {JUL,31,"Anniversary of Battle of Passchendaele"                  } ,
   {JUL,31,"Primo Levi's birthday"                                   } ,
   {JUL,31,"Joanne Kathleen Rowling's birthday"                      } ,
   {JUL,31,"Harry Potter's birthday"                                 } ,

   {AUG, 1,"Anniversary of Warsaw Uprising"                          } ,
   {AUG, 1,"Dahomey Independence Day"                                } ,
   {AUG, 1,"Switzerland Confederation Day"                           } ,
   {AUG, 1,"Jerry Garcia's birthday"                                 } ,
   {AUG, 1,"IPA Day - bottoms up!"                                   } ,
   {AUG, 2,"Anniversary of Battle of Cannae"                         } ,
   {AUG, 3,"International Beer Day!"                                 } ,
   {AUG, 4,"Percy Bysshe Shelley's birthday"                         } ,
   {AUG, 4,"Founding of US Coast Guard (1790)"                       } ,
   {AUG, 5,"Upper Volta Independence Day"                            } ,
   {AUG, 5,"Neil Armstrong's birthday"                               } ,
   {AUG, 6,"Bolivia Independence Day"                                } ,
   {AUG, 6,"Alexander Fleming's birthday"                            } ,
   {AUG, 7,"Ivory Coast Independence Day"                            } ,
   {AUG, 8,"P A M Dirac's birthday"                                  } ,
   {AUG, 8,"World Cat Day (meow)"                                    } ,
   {AUG, 9,"Anniversary of Richard Nixon's resignation"              } ,
   {AUG, 9,"Singapore National Day"                                  } ,
   {AUG,10,"Ecuador Independence Day"                                } ,
   {AUG,11,"Anniversary of Alcatraz Prison"                          } ,
   {AUG,11,"Anniversary of Battle of Thermopylae"                    } ,
   {AUG,12,"Erwin Schrodinger's birthday"                            } ,
   {AUG,12,"World Elephant Day!"                                     } ,
   {AUG,14,"Pakistan Independence Day"                               } ,
   {AUG,15,"Napoleon's birthday"                                     } ,
   {AUG,15,"India Independence Day"                                  } ,
   {AUG,15,"Liechtenstein National Day"                              } ,
   {AUG,16,"Cyprus Independence Day"                                 } ,
   {AUG,17,"Pierre de Fermat's birthday"                             } ,
   {AUG,17,"Gabon Independence Day"                                  } ,
   {AUG,17,"Indonesia Independence Day"                              } ,
   {AUG,18,"Afghanistan Independence Day"                            } ,
   {AUG,19,"Gene Roddenberry's birthday"                             } ,
   {AUG,20,"Waclaw Sierpinski's birthday"                            } ,
   {AUG,22,"Anniversary of Battle of Bosworth Field"                 } ,
   {AUG,23,"Rumania Liberation Day"                                  } ,
   {AUG,23,"Gene Kelley's birthday"                                  } ,
   {AUG,24,"William Wilberforce's birthday"                          } ,
   {AUG,25,"Uruguay Independence Day"                                } ,
   {AUG,26,"Anniversary of 19th Amendment's adoption"                } ,
   {AUG,26,"National Dog Day (USA - woof)"                           } ,
   {AUG,26,"Feast Day for Our Lady of Czestochowa"                   } ,
   {AUG,26,"Herero Day (Namibia)"                                    } ,
   {AUG,27,"Confucius' birthday"                                     } ,
   {AUG,27,"Charles Gates Dawes' birthday"                           } ,
   {AUG,30,"Olga Taussky-Todd's birthday"                            } ,
   {AUG,31,"Anniversary of Solidarity's creation"                    } ,
   {AUG,31,"Trinidad & Tobago Independence Day"                      } ,

   {SEP, 1,"Anniversary of Nazi invasion of Poland"                  } ,
   {SEP, 2,"VJ Day"                                                  } ,
   {SEP, 2,"Anniversary of Battle of Actium"                         } ,
   {SEP, 2,"Queen Liliuokalani's birthday"                           } ,
   {SEP, 3,"Qatar Independence Day"                                  } ,
   {SEP, 3,"San Marino Anniversary of Founding"                      } ,
   {SEP, 3,"Anniversary of Treaty of Paris, 1783"                    } ,
   {SEP, 4,"First hard disk's birthday"                              } ,
   {SEP, 6,"Swaziland Independence Day"                              } ,
   {SEP, 7,"Brazil Independence Day"                                 } ,
   {SEP, 7,"Anniversary of Battle of Borodino"                       } ,
   {SEP, 7,"Queen Elizabeth I's birthday"                            } ,
   {SEP, 7,"Sinclair Lewis's birthday"                               } ,
   {SEP, 7,"Buddy Holly's birthday"                                  } ,
   {SEP, 7,"Luigi Galvani's birthday"                                } ,
   {SEP, 7,"Birthday of Emperor Saga (Japan)"                        } ,
   {SEP, 7,"Celebrate Emperor Zhark Day!"                            } ,
   {SEP, 8,"Anniversary of Star Trek TV debut"                       } ,
   {SEP, 8,"Andorra National Festival"                               } ,
   {SEP, 9,"Bulgaria Liberation Day"                                 } ,
   {SEP,11,"9/11"                                                    } ,
   {SEP,12,"Anniversary of Battle of Vienna"                         } ,
   {SEP,14,"Jan Masaryk's birthday"                                  } ,
   {SEP,14,"Anniversary of Friston & Cox Debut"                      } ,
   {SEP,15,"Costa Rica Independence Day"                             } ,
   {SEP,15,"El Salvador Independence Day"                            } ,
   {SEP,15,"Guatemala Independence Day"                              } ,
   {SEP,15,"Honduras Independence Day"                               } ,
   {SEP,15,"Nicaragua Independence Day"                              } ,
   {SEP,16,"Malaysia Independence Day"                               } ,
   {SEP,16,"Papua New Guinea Independence Day"                       } ,
   {SEP,17,"Anniversary of signing of American Constitution"         } ,
   {SEP,17,"Georg Bernhard Riemann's birthday"                       } ,
   {SEP,17,"Anniversary of Battle of Antietam"                       } ,
   {SEP,17,"Anniversary of Soviet invasion of Poland"                } ,
   {SEP,18,"Mexico Independence Day"                                 } ,
   {SEP,18,"Founding of US Air Force (1947)"                         } ,
   {SEP,19,"Anniversary of Battle of Saratoga"                       } ,
   {SEP,19,"Hermione Granger's birthday"                             } ,
   {SEP,20,"Anniversary of Battle of Chalons"                        } ,
   {SEP,21,"Chile Independence Day"                                  } ,
   {SEP,21,"Belize Independence Day"                                 } ,
   {SEP,21,"Malta Independence Day"                                  } ,
   {SEP,22,"Bilbo & Frodo Baggins' birthday"                         } ,
   {SEP,22,"Hug a Rhinoceros (carefully) Day"                        } ,
   {SEP,22,"Mali Republic Day"                                       } ,
   {SEP,23,"Saudi Arabia National Day"                               } ,
   {SEP,23,"Emperor Augustus Caesar's birthday"                      } ,
   {SEP,26,"Yemen National Day"                                      } ,
   {SEP,27,"Anniversary of founding of Jesuit Order"                 } ,
   {SEP,28,"Anniversary of discovery of penicillin"                  } ,
   {SEP,28,"Seymour Cray's birthday"                                 } ,
   {SEP,29,"Lech Walesa's birthday"                                  } ,
   {SEP,30,"Botswana Independence Day!"                              } ,

   {OCT, 1,"Nigeria Independence Day"                                } ,
   {OCT, 2,"Guinea Independence Day"                                 } ,
   {OCT, 2,"Mohandas Karamchand Gandhi's Birthday"                   } ,
   {OCT, 2,"Anniversary of Saladin's capture of Jerusalem"           } ,
   {OCT, 2,"Groucho Marx's birthday"                                 } ,
   {OCT, 3,"Anniversary of reunification of Germany"                 } ,
   {OCT, 4,"Anniversary of Sputnik launch"                           } ,
   {OCT, 4,"Houston Bootcamp Anniversary"                            } ,
   {OCT, 6,"Anniversary of attack on Frodo at Weathertop"            } ,
   {OCT, 9,"Alfred Dreyfus's birthday"                               } ,
   {OCT,10,"Anniversary of Battle of Tours"                          } ,
   {OCT,12,"Columbus Day"                                            } ,
   {OCT,12,"Spain National Day"                                      } ,
   {OCT,13,"Founding of US Navy (1775)"                              } ,
   {OCT,14,"Dwight Eisenhower's birthday"                            } ,
   {OCT,15,"Pelham Grenville Wodehouse's birthday"                   } ,
   {OCT,16,"National Boss Day!"                                      } ,
   {OCT,16,"Hamilton Day!"                                           } ,
   {OCT,19,"Anniversary of Cornwallis' surrender at Yorktown"        } ,
   {OCT,20,"World Statistics Day!"                                   } ,
   {OCT,21,"Anniversary of Battle of Trafalgar"                      } ,
   {OCT,24,"Anniversary of founding of United Nations"               } ,
   {OCT,24,"Zambia Independence Day"                                 } ,
   {OCT,25,"Saint Crispin's (or Crispian's) Day"                     } ,
   {OCT,25,"Evariste Galois' birthday"                               } ,
   {OCT,26,"Anniversary of Gunfight at the OK Corral"                } ,
   {OCT,27,"Theodore Roosevelt's birthday"                           } ,
   {OCT,27,"John Cleese's birthday"                                  } ,
   {OCT,28,"Anniversary of Battle of Milvian Bridge"                 } ,
   {OCT,29,"Turkey Republic Day"                                     } ,
   {OCT,29,"Anniversary of Black Tuesday"                            } ,
   {OCT,30,"Anniversary of the War of the Worlds!"                   } ,
   {OCT,30,"Seoul SNUH Bootcamp Anniversary"                         } ,
   {OCT,31,"Halloween"                                               } ,
   {OCT,31,"John Keats' birthday"                                    } ,
   {OCT,31,"Karl Weierstrass's birthday"                             } ,

   {NOV, 1,"All Saint's Day"                                         } ,  /* WW */
   {NOV, 1,"Anniversary of Great Lisbon Earthquake"                  } ,
   {NOV, 2,"Marie Antoinette's birthday"                             } ,
   {NOV, 2,"Anniversary of Balfour Declaration"                      } ,
   {NOV, 2,"Daniel Boone's birthday"                                 } ,
   {NOV, 3,"Panama Independence Day"                                 } ,
   {NOV, 3,"Jellyfish Day"                                           } ,
   {NOV, 3,"Dominica Independence Day"                               } ,
   {NOV, 4,"Art Carney's birthday"                                   } ,
   {NOV, 5,"Guy Fawkes Day"                                          } ,
   {NOV, 6,"John Philip Sousa's birthday"                            } ,
   {NOV, 6,"Dominican Republic Constitution Day"                     } ,
   {NOV, 7,"Maria Sklodowska Curie's birthday"                       } ,
   {NOV, 7,"Lise Meitner's birthday"                                 } ,
   {NOV, 8,"Anniversary of discovery of X-rays"                      } ,
   {NOV, 8,"Anniversary of Louvre museum opening"                    } ,
   {NOV, 8,"Edmond Halley's birthday"                                } ,
   {NOV, 8,"Bram Stoker's birthday"                                  } ,
   {NOV, 8,"Felix Haussdorf's birthday"                              } ,
   {NOV, 8,"Katherine Hepburn's birthday"                            } ,
   {NOV, 9,"Benjamin Banneker's birthday"                            } ,
   {NOV, 9,"Tainan NCKU Bootcamp Anniversary"                        } ,
   {NOV, 9,"Cambodia Independence Day"                               } ,
   {NOV,10,"Martin Luther's birthday"                                } ,
   {NOV,10,"Founding of US Marine Corps (1775)"                      } ,
   {NOV,11,"Veteran's & Remembrance Day"                             } ,
   {NOV,11,"Anniversary of Sherman's March to the Sea"               } ,
   {NOV,11,"Poland Independence Day"                                 } ,
   {NOV,12,"Sun Yat Sen's birthday"                                  } ,
   {NOV,12,"Elizabeth Cady Stanton's birthday"                       } ,
   {NOV,12,"Auguste Rodin's birthday"                                } ,
   {NOV,12,"Comoros Independence Day"                                } ,
   {NOV,13,"Saint Augustine's birthday"                              } ,
   {NOV,13,"James Clerk Maxwell's birthday"                          } ,
   {NOV,13,"Robert Louis Stevenson's birthday"                       } ,
   {NOV,14,"Claude Monet's birthday"                                 } ,
   {NOV,16,"James Bonds' birthday"                                   } ,
   {NOV,16,"d'Alembert's birthday"                                   } ,
   {NOV,16,"Saint Malgorzata's (of Scotland) Day"                    } ,
   {NOV,17,"Korbinian Brodmann's birthday"                           } ,
   {NOV,18,"Ignacy Paderewski's birthday"                            } ,
   {NOV,19,"Anniversary of Gettysburg Address"                       } ,
   {NOV,19,"Monaco National Fete"                                    } ,
   {NOV,19,"World Toilet Day"                                        } ,
   {NOV,20,"Edwin Hubble's birthday"                                 } ,
   {NOV,20,"National Absurdity Day"                                  } ,
   {NOV,21,"Francois-Marie Arouet's birthday"                        } ,
   {NOV,22,"Anniversary of Kennedy's assasination"                   } ,
   {NOV,22,"Lebanon Independence Day"                                } ,
   {NOV,22,"Charles de Gaulle's birthday"                            } ,
   {NOV,23,"Anniversary of Dr Who's debut"                           } ,
   {NOV,23,"Billy the Kid's birthday"                                } ,
   {NOV,23,"Boris Karloff's birthday"                                } ,
   {NOV,23,"Harpo Marx's birthday"                                   } ,
   {NOV,24,"Baruch Spinoza's birthday"                               } ,
   {NOV,24,"Anniversary of Tasmania's discovery"                     } ,
   {NOV,24,"Anniversary of Darwin's The Origin of Species"           } ,
   {NOV,25,"Pope John XXIII's birthday"                              } ,
   {NOV,25,"Joe DiMaggio's birthday"                                 } ,
   {NOV,25,"Suriname Independence Day"                               } ,
   {NOV,25,"G = 8*Pi*T day"                                          } ,
   {NOV,26,"Anniversary of Casablanca's debut"                       } ,
   {NOV,26,"\"Rick, you've got to help me, Rick!\""                  } ,
   {NOV,26,"Norbert Wiener's birthday"                               } ,
   {NOV,27,"Anniversary of First Crusade's start"                    } ,
   {NOV,27,"Chaim Weizmann's birthday"                               } ,
   {NOV,27,"Aleksander Dubcek's birthday"                            } ,
   {NOV,28,"Mauritania Independence Day"                             } ,
   {NOV,29,"Clive Staples Lewis's birthday"                          } ,
   {NOV,29,"Jacques Chirac's birthday"                               } ,
   {NOV,30,"Samuel Clemens' birthday"                                } ,
   {NOV,30,"Winston Churchill's birthday"                            } ,
   {NOV,30,"Barbados Independence Day"                               } ,
   {NOV,30,"Saint Andrew's Day"                                      } ,
   {NOV,30,"Barbados Independence Day"                               } ,
   {NOV,30,"Jagdish Chandra Bose's birthday"                         } ,

   {DEC, 1,"Anniversary of Rosa Parks' arrest"                       } ,  /* WW */
   {DEC, 1,"Woody Allen's birthday"                                  } ,
   {DEC, 1,"Central African Republic National Day"                   } ,
   {DEC, 2,"Anniversary of Monroe Doctrine"                          } ,
   {DEC, 2,"Georges Seurat's birthday"                               } ,
   {DEC, 2,"Anniversary of Napoleon's coronation"                    } ,
   {DEC, 2,"Anniversary of Battle of Austerlitz"                     } ,
   {DEC, 3,"Joseph Konrad's birthday"                                } ,
   {DEC, 3,"Anniversary of Galileo's telescope"                      } ,
   {DEC, 4,"Crazy Horse's birthday"                                  } ,
   {DEC, 5,"Anniversary of Prohibition's end!"                       } ,
   {DEC, 5,"George Armstrong Custer's birthday"                      } ,
   {DEC, 5,"Werner Karl Heisenberg's birthday"                       } ,
   {DEC, 5,"Jozef Pilsudki's birthday"                               } ,
   {DEC, 6,"Spanish Constitution Day"                                } ,
   {DEC, 6,"Finland Independence Day"                                } ,
   {DEC, 6,"Anniversary of the Irish Free State"                     } ,
   {DEC, 6,"Henryk Gorecki's birthday"                               } ,
   {DEC, 6,"Anniversary of 13th Amendment!"                          } ,
   {DEC, 7,"Anniversary of Battle of Pearl Harbor"                   } ,
   {DEC, 7,"Ivory Coast National Day"                                } ,
   {DEC, 8,"Jacques Hadamard's birthday"                             } ,
   {DEC, 8,"Jim Morrison's birthday"                                 } ,
   {DEC, 8,"Diego Rivera's birthday"                                 } ,
   {DEC, 8,"Anniversary of Syllabus Errorum"                         } ,
   {DEC, 8,"Anniversary of US Declaration of War on Japan"           } ,
   {DEC, 9,"John Milton's birthday"                                  } ,
   {DEC, 9,"Anniversary of Lech Walesa's election"                   } ,
   {DEC, 9,"Tanzania Independence Day"                               } ,
   {DEC,10,"Ada Lovelace's birthday"                                 } ,
   {DEC,10,"Anniversary of Spanish-American War's end"               } ,
   {DEC,10,"Anniversary of Grateful Dead's first concert"            } ,
   {DEC,10,"Emily Dickinson's birthday"                              } ,
   {DEC,10,"Anniversary of Female Suffrage in Wyoming"               } ,
   {DEC,11,"Robert Koch's birthday"                                  } ,
   {DEC,11,"Upper Volta Republic Day"                                } ,
   {DEC,11,"Aleksander Solzhenitsyn's birthday"                      } ,
   {DEC,11,"Max Born's birthday"                                     } ,
   {DEC,11,"Fiorello LaGuardia's birthday"                           } ,
   {DEC,12,"Frank Sinatra's birthday"                                } ,
   {DEC,12,"Kenya Independence Day"                                  } ,
   {DEC,12,"William Lloyd Garrison's birthday"                       } ,
   {DEC,12,"Edvard Munch's birthday"                                 } ,
   {DEC,13,"Werner von Siemens's birthday"                           } ,
   {DEC,13,"Anniversary of Tasman at New Zealand"                    } ,
   {DEC,14,"Tycho Brahe's birthday"                                  } ,
   {DEC,14,"Nostradamus's birthday"                                  } ,
   {DEC,14,"Jimmy Doolittle's birthday"                              } ,
   {DEC,15,"Alexandre Eiffel's birthday"                             } ,
   {DEC,15,"Anniversary of adoption of US Bill of Rights"            } ,
   {DEC,15,"Anniversary of Gone With the Wind debut"                 } ,
   {DEC,16,"Anniversary of Boston Tea Party"                         } ,
   {DEC,16,"Jane Austen's birthday"                                  } ,
   {DEC,16,"Arthur C Clarke's birthday"                              } ,
   {DEC,17,"Anniversary of Wright brother's flight"                  } ,
   {DEC,17,"Beethoven's birthday"                                    } ,
   {DEC,17,"Roman Saturnalia!"                                       } ,
   {DEC,17,"Bhutan National Day"                                     } ,
   {DEC,18,"Anniversary of 13th Amendment's adoption"                } ,
   {DEC,18,"Niger Independence Day"                                  } ,
   {DEC,19,"William Pitt's (the younger) birthday"                   } ,
   {DEC,20,"Robert Menzies' birthday"                                } ,
   {DEC,20,"Anniversary of Louisiana Purchase"                       } ,
   {DEC,21,"Anniversary of Pilgrim's landing"                        } ,
   {DEC,21,"Thomas Becket's birthday"                                } ,
   {DEC,21,"Jan Lukasiewicz' birthday"                               } ,
   {DEC,21,"Nepal Independence Day"                                  } ,
   {DEC,22,"Srinivasa Ramanujan's birthday"                          } ,
   {DEC,23,"Emperor Akihito's birthday"                              } ,
   {DEC,24,"Adam Mickiewicz's birthday"                              } ,
   {DEC,24,"Ignatius Loyola's birthday"                              } ,
   {DEC,24,"Kit Carson's birthday"                                   } ,
   {DEC,24,"Christmas Eve"                                           } ,
   {DEC,25,"Christmas Day - Ho Ho Ho"                                } ,
   {DEC,25,"Sir Isaac Newton's birthday"                             } ,
   {DEC,25,"Clara Barton's birthday"                                 } ,
   {DEC,25,"Rod Serling's birthday"                                  } ,
   {DEC,25,"Anwar Sadat's birthday"                                  } ,
   {DEC,25,"Anniversary of Washingon's crossing the Delaware"        } ,
   {DEC,26,"Charles Babbage's birthday"                              } ,
   {DEC,26,"Boxing Day"                                              } ,
   {DEC,27,"Johannes Kepler's birthday"                              } ,
   {DEC,27,"Louis Pasteur's birthday"                                } ,
   {DEC,27,"Anniversary of Indonesian Independence"                  } ,
   {DEC,28,"Arthur Eddington's birthday"                             } ,
   {DEC,28,"John von Neumann's birthday"                             } ,
   {DEC,28,"Linus Torvalds' birthday"                                } ,
   {DEC,29,"William Gladstone's birthday"                            } ,
/**   {DEC,30,"Tiger Woods' birthday"                                   } , **/
   {DEC,30,"Rudyard Kipling's birthday"                              } ,
   {DEC,31,"New Year's Eve"                                          } ,
   {DEC,31,"George C Marshall's birthday"                            } ,
   {DEC,31,"Simon Wiesenthal's birthday"                             } ,

   /* some other stuff from Wikipedia */

   {JAN,  1, "Global Family Day"   } ,
   {JAN,  1, "World Day of Peace"   } ,
   {JAN, 31, "Street Children's Day"   } ,
   {FEB,  2, "World Wetlands Day"   } ,
   {FEB,  4, "World Cancer Day"   } ,
   {FEB, 11, "World Day of the Sick"   } ,
   {FEB, 13, "World Radio Day"   } ,
   {FEB, 20, "World Day of Social Justice"   } ,
   {MAR, 14, "Pi Day (celebrate at 4pm)"   } ,
   {MAR, 20, "World Sparrow Day"   } ,
   {MAR, 21, "International Day of Nowruz"   } ,
   {MAR, 21, "World Down Syndrome Day"   } ,
   {MAR, 21, "World Puppetry Day"   } ,
   {MAR, 21, "International Day of Forests"   } ,
   {MAR, 22, "World Water Day"   } ,
   {MAR, 23, "World Meteorological Day"   } ,
   {MAR, 24, "World Tuberculosis Day"   } ,
   {MAR, 31, "International Cigar Day"   } ,
   {APR,  2, "World Autism Awareness Day"   } ,
   {APR,  7, "World Health Day"   } ,
   {APR,  8, "International Romani Day"   } ,
   {APR, 16, "World Entrepreneurship Day"   } ,
   {APR, 17, "World Hemophilia Day"   } ,
   {APR, 22, "International Mother Earth Day"   } ,
   {APR, 24, "World Day for Laboratory Animals"   } ,
   {APR, 25, "World Malaria Day"   } ,
   {APR, 30, "International Jazz Day"   } ,
   {MAY,  5, "Day of Vesak"   } ,
   {MAY,  5, "International Midwives Day"   } ,
   {MAY, 12, "World Migratory Bird Day"   } ,
   {MAY, 12, "International Nurses Day"   } ,
   {MAY, 15, "International Day of Families"   } ,
   {MAY, 18, "International Museum Day"   } ,
   {MAY, 22, "World Goth Day"   } ,
   {MAY, 23, "World Turtle Day"   } ,
   {MAY, 25, "Geek Pride Day"   } ,
   {MAY, 31, "World No-Tobacco Day"   } ,
   {JUN,  1, "International Children's Day"   } ,
   {JUN,  5, "World Environment Day"   } ,
   {JUN,  8, "World Oceans Day"   } ,
   {JUN,  8, "World Brain Tumor Day"   } ,
   {JUN, 10, "Ballpoint Pen Day"   } ,
   {JUN, 14, "World Blood Donor Day"   } ,
   {JUN, 18, "Autistic Pride Day"   } ,
   {JUN, 19, "World Sickle Cell Day"   } ,
   {JUN, 20, "World Refugee Day"   } ,
   {JUN, 21, "World Music Day"   } ,
   {JUN, 23, "International Widow's Day"   } ,
   {JUN, 25, "Day of the Seafarer"   } ,
   {JUL,  2, "World UFO Day"   } ,
   {JUL,  8, "Writer's Day"   } ,
   {JUL, 11, "World Population Day"   } ,
   {JUL, 29, "International Tiger Day"   } ,
   {AUG,  5, "International Beer Day"   } ,
   {AUG,  7, "International Hipster Day"   } ,
   {AUG, 12, "International Youth Day"   } ,
   {AUG, 13, "International Lefthanders Day"   } ,
   {AUG, 19, "World Humanitarian Day"   } ,
   {AUG, 19, "World Photography Day"   } ,
   {AUG, 21, "World Fashion Day"   } ,
   {AUG, 24, "World Hip Hop Day"   } ,
   {SEP,  8, "International Literacy Day"   } ,
   {SEP,  8, "World Physical Therapy Day"   } ,
   {SEP, 19, "Talk Like a Pirate Day"   } ,
   {SEP, 20, "International Day of Freedom"   } ,
   {SEP, 21, "International Day of Peace"   } ,
   {SEP, 21, "World Alzheimer's Day"   } ,
   {SEP, 22, "World Rhino Day"   } ,
   {SEP, 27, "World Tourism Day"   } ,
   {SEP, 28, "World Rabies Day"   } ,
   {SEP, 30, "World Helping Day"   } ,
   {SEP, 30, "Blasphemy Day"   } ,
   {OCT,  1, "World Vegetarian Day"   } ,
   {OCT,  2, "International Day of Non-Violence"   } ,
   {OCT,  4, "World Animal Day"   } ,
   {OCT,  5, "World Teachers' Day"   } ,
   {OCT,  8, "World Humanitarian Action Day"   } ,
   {OCT,  9, "World Post Day"   } ,
   {OCT, 10, "World Mental Health Day"   } ,
   {OCT, 12, "World Arthritis Day"   } ,
   {OCT, 14, "World Standards Day"   } ,
   {OCT, 15, "Global Handwashing Day"   } ,
   {OCT, 16, "World Food Day"   } ,
   {OCT, 20, "World Osteoporosis Day"   } ,
   {OCT, 20, "World Statistics Day"   } ,
   {OCT, 24, "United Nations Day"   } ,
   {OCT, 28, "INTERNATIONAL CAPS LOCK DAY"   } ,
   {OCT, 28, "International Animation Day"   } ,
   {NOV, 12, "World Pneumonia Day"   } ,
   {NOV, 13, "World Kindness Day"   } ,
   {NOV, 14, "World Diabetes Day"   } ,
   {NOV, 15, "Day of the Imprisoned Writer"   } ,
   {NOV, 16, "International Day for Tolerance"   } ,
   {NOV, 17, "International Students Day"   } ,
   {NOV, 20, "Universal Children's Day"   } ,
   {NOV, 21, "World Television Day"   } ,
   {NOV, 21, "World Fisheries Day"   } ,
   {NOV, 21, "World Hello Day"   } ,
   {NOV, 30, "Computer Security Day"   } ,
   {DEC,  9, "International Anti-Corruption Day"   } ,
   {DEC, 10, "Human Rights Day"   } ,
   {DEC, 11, "International Mountain Day"   } ,
   {DEC, 13, "International Day Day"   } ,
   {DEC, 18, "International Migrants Day"   } ,

   /* US State Admissions [11 May 2015] */

   {DEC, 7,"Anniversary of Delaware Admission"        } ,
   {DEC,12,"Anniversary of Pennsylvania Admission"    } ,
   {DEC,18,"Anniversary of New Jersey Admission"      } ,
   {JAN, 2,"Anniversary of Georgia Admission"         } ,
   {JAN, 9,"Anniversary of Connecticut Admission"     } ,
   {FEB, 6,"Anniversary of Massachusetts Admission"   } ,
   {APR,28,"Anniversary of Maryland Admission"        } ,
   {MAY,23,"Anniversary of South Carolina Admission"  } ,
   {JUN,21,"Anniversary of New Hampshire Admission"   } ,
   {JUN,25,"Anniversary of Virginia Admission"        } ,
   {JUL,26,"Anniversary of New York Admission"        } ,
   {NOV,21,"Anniversary of North Carolina Admission"  } ,
   {MAY,29,"Anniversary of Rhode Island Admission"    } ,
   {MAR, 4,"Anniversary of Vermont Admission"         } ,
   {JUN, 1,"Anniversary of Kentucky Admission"        } ,
   {JUN, 1,"Anniversary of Tennessee Admission"       } ,
   {MAR, 1,"Anniversary of Ohio Admission"            } ,
   {APR,30,"Anniversary of Louisiana Admission"       } ,
   {DEC,11,"Anniversary of Indiana Admission"         } ,
   {DEC,10,"Anniversary of Mississippi Admission"     } ,
   {DEC, 3,"Anniversary of Illinois Admission"        } ,
   {DEC,14,"Anniversary of Alabama Admission"         } ,
   {MAR,15,"Anniversary of Maine Admission"           } ,
   {AUG,10,"Anniversary of Missouri Admission"        } ,
   {JUN,15,"Anniversary of Arkansas Admission"        } ,
   {JAN,26,"Anniversary of Michigan Admission"        } ,
   {MAR, 3,"Anniversary of Florida Admission"         } ,
   {DEC,29,"Anniversary of Texas Admission"           } ,
   {DEC,28,"Anniversary of Iowa Admission"            } ,
   {MAY,29,"Anniversary of Wisconsin Admission"       } ,
   {SEP, 9,"Anniversary of California Admission"      } ,
   {MAY,11,"Anniversary of Minnesota Admission"       } ,
   {FEB,14,"Anniversary of Oregon Admission"          } ,
   {JAN,29,"Anniversary of Kansas Admission"          } ,
   {JUN,20,"Anniversary of West Virginia Admission"   } ,
   {OCT,31,"Anniversary of Nevada Admission"          } ,
   {MAR, 1,"Anniversary of Nebraska Admission"        } ,
   {AUG, 1,"Anniversary of Colorado Admission"        } ,
   {NOV, 2,"Anniversary of North Dakota Admission"    } ,
   {NOV, 2,"Anniversary of South Dakota Admission"    } ,
   {NOV, 8,"Anniversary of Montana Admission"         } ,
   {NOV,11,"Anniversary of Washington Admission"      } ,
   {JUL, 3,"Anniversary of Idaho Admission"           } ,
   {JUL,10,"Anniversary of Wyoming Admission"         } ,
   {JAN, 4,"Anniversary of Utah Admission"            } ,
   {NOV,16,"Anniversary of Oklahoma Admission"        } ,
   {JAN, 6,"Anniversary of New Mexico Admission"      } ,
   {FEB,14,"Anniversary of Arizona Admission"         } ,
   {JAN, 3,"Anniversary of Alaska Admission"          } ,
   {AUG,21,"Anniversary of Hawaii Admission"          } ,

   /* cf. http://www.theonlinerecipebox.com/national-food-holidays/ */

   {JAN, 1, "Celebrate National Soup Month" } ,
   {JAN, 2, "National Cream Puff Day" } ,
   {JAN, 3, "National Chocolate Covered Cherry Day" } ,
   {JAN, 4, "National Spaghetti Day" } ,
   {JAN, 5, "National Whipped Cream Day" } ,
   {JAN, 6, "National Shortbread Day" } ,
   {JAN, 7, "Celebrate National Oatmeal Month" } ,
   {JAN, 8, "National English Toffee Day" } ,
   {JAN, 9, "National Apricot Day" } ,
   {JAN, 10, "National Bittersweet Chocolate Day" } ,
   {JAN, 11, "National Milk Day" } ,
   {JAN, 12, "National Curried Chicken Day" } ,
   {JAN, 13, "National Peach Melba Day" } ,
   {JAN, 14, "National Hot Pastrami Sandwich Day" } ,
   {JAN, 15, "National Strawberry Ice Cream Day" } ,
   {JAN, 16, "National Fig Newton Day" } ,
   {JAN, 17, "Celebrate National Wheat Bread Month" } ,
   {JAN, 18, "National Peking Duck Day" } ,
   {JAN, 19, "National Popcorn Day" } ,
   {JAN, 20, "National Granola Bar Day" } ,
   {JAN, 21, "National New England Clam Chowder Day" } ,
   {JAN, 22, "National Blonde Brownie Day" } ,
   {JAN, 23, "National Rhubarb Pie Day" } ,
   {JAN, 24, "National Peanut Butter Day" } ,
   {JAN, 25, "Celebrate National Egg Month" } ,
   {JAN, 26, "National Pistachio Day" } ,
   {JAN, 27, "National Chocolate Cake Day" } ,
   {JAN, 28, "National Blueberry Pancake Day" } ,
   {JAN, 29, "National Corn Chip Day" } ,
   {JAN, 30, "National Croissant Day" } ,
   {JAN, 31, "Celebrate National Meat Month" } ,
   {FEB, 1, "National Baked Alaska Day" } ,
   {FEB, 2, "National Heavenly Hash Day" } ,
   {FEB, 3, "National Carrot Cake Day" } ,
   {FEB, 4, "National Homemade Soup Day" } ,
   {FEB, 5, "National Chocolate Fondue Day" } ,
   {FEB, 6, "celebrate National Sweet Potato Month" } ,
   {FEB, 7, "National Fettuccini Alfredo Day" } ,
   {FEB, 8, "National Molasses Bar Day" } ,
   {FEB, 9, "National Pistachio Day" } ,
   {FEB, 10, "National Pizza Pie Day" } ,
   {FEB, 11, "National Peppermint Patty Day" } ,
   {FEB, 12, "National Plum Pudding Day" } ,
   {FEB, 13, "National Tortellini Day" } ,
   {FEB, 14, "National Cream Filled Chocolates Day" } ,
   {FEB, 15, "National Gum Drop Day" } ,
   {FEB, 16, "National Almond Day" } ,
   {FEB, 17, "National Indian Pudding Day" } ,
   {FEB, 18, "National Crab Stuffed Flounder Day" } ,
   {FEB, 19, "National Chocolate Mint Day" } ,
   {FEB, 20, "National Cherry Pie Day" } ,
   {FEB, 21, "National Sticky Bun Day" } ,
   {FEB, 22, "Celebrate National Grapefruit Month" } ,
   {FEB, 23, "National Banana Bread Day" } ,
   {FEB, 24, "National Tortilla Chip Day" } ,
   {FEB, 25, "National Clam Chowder Day" } ,
   {FEB, 26, "National Pistachio Day" } ,
   {FEB, 27, "National Strawberry Day" } ,
   {FEB, 28, "National Chocolate Souffle Day" } ,
   {FEB, 29, "Celebrate National Cherry Month" } ,
   {MAR, 1, "National Peanut Butter Lover's Day" } ,
   {MAR, 2, "National Banana Cream Pie Day" } ,
   {MAR, 3, "National Cold Cuts Day" } ,
   {MAR, 4, "National Pound Cake Day" } ,
   {MAR, 5, "National Cheese Doodle Day" } ,
   {MAR, 6, "National White Chocolate Cheesecake Day" } ,
   {MAR, 7, "National Cereal Day" } ,
   {MAR, 8, "National Peanut Cluster Day" } ,
   {MAR, 9, "National Crab Meat Day" } ,
   {MAR, 10, "National Blueberry Popover Day" } ,
   {MAR, 11, "National Oatmeal Nuts Waffles Day" } ,
   {MAR, 12, "National Baked Scallops Day" } ,
   {MAR, 13, "National Coconut Torte Day" } ,
   {MAR, 14, "National Potato Chip Day" } ,
   {MAR, 15, "National Pears Helene Day" } ,
   {MAR, 16, "National Artichoke Hearts Day" } ,
   {MAR, 17, "National Corned Beef and Cabbage Day" } ,
   {MAR, 18, "National Oatmeal Cookie Day" } ,
   {MAR, 19, "National Chocolate Caramel Day" } ,
   {MAR, 20, "National Ravioli Day" } ,
   {MAR, 21, "National California Strawberry Day" } ,
   {MAR, 22, "Celebrate National Celery Month" } ,
   {MAR, 23, "National Melba Toast Day" } ,
   {MAR, 24, "National Chocolate Covered Raisins Day" } ,
   {MAR, 25, "National Waffle Day" } ,
   {MAR, 26, "National Spinach Day" } ,
   {MAR, 27, "National Spanish Paella Day" } ,
   {MAR, 28, "National Black Forest Cake Day" } ,
   {MAR, 29, "National Lemon Chiffon Cake Day" } ,
   {MAR, 30, "Celebrate National Peanut Month" } ,
   {MAR, 31, "National Tater Day" } ,
   {APR, 1, "National Sourdough Bread Day" } ,
   {APR, 2, "National Peanut Butter and Jelly Day" } ,
   {APR, 3, "National Chocolate Mousse Day" } ,
   {APR, 4, "National Cordon Bleu Day" } ,
   {APR, 5, "National Raisin and Spice Bar Day" } ,
   {APR, 6, "National Caramel Popcorn Day" } ,
   {APR, 7, "Celebrate National Florida Tomato Month" } ,
   {APR, 8, "National Empanada Day" } ,
   {APR, 9, "National Chinese Almond Cookie Day" } ,
   {APR, 10, "National Cinnamon Crescent Day" } ,
   {APR, 11, "National Cheese Fondue Day" } ,
   {APR, 12, "National Grilled Cheese Sandwich Day" } ,
   {APR, 13, "National Peach Cobbler Day" } ,
   {APR, 14, "National Pecan Day" } ,
   {APR, 15, "National Glazed Spiral Ham Day" } ,
   {APR, 16, "National Eggs Benedict Day" } ,
   {APR, 17, "National Cheese Ball Day" } ,
   {APR, 18, "Animal Crackers Birthday" } ,
   {APR, 19, "National Garlic Day" } ,
   {APR, 20, "National Pineapple Upside-down Cake Day" } ,
   {APR, 21, "National Chocolate-covered Cashew Truffle Day" } ,
   {APR, 22, "National Jelly Bean Day" } ,
   {APR, 23, "National Cherry Cheesecake Day" } ,
   {APR, 24, "National Pigs-in-a-Blanket Day" } ,
   {APR, 25, "National Zucchini Bread Day" } ,
   {APR, 26, "National Pretzel Day" } ,
   {APR, 27, "National Prime Rib Day" } ,
   {APR, 28, "National Blueberry Pie Day" } ,
   {APR, 29, "National Shrimp Scampi Day" } ,
   {APR, 30, "National Raisin Day" } ,
   {MAY, 1, "National Chocolate Parfait Day" } ,
   {MAY, 2, "National Truffles Day" } ,
   {MAY, 3, "National Raspberry Tart Day" } ,
   {MAY, 4, "National Orange Juice Day" } ,
   {MAY, 5, "National Chocolate Custard Day" } ,
   {MAY, 6, "National Crepes Suzette Day" } ,
   {MAY, 7, "National Roast Leg of Lamb Day" } ,
   {MAY, 8, "Celebrate National Salsa Month" } ,
   {MAY, 9, "National Butterscotch Brownie Day" } ,
   {MAY, 10, "National Shrimp Day" } ,
   {MAY, 11, "Celebrate National Gazpacho Avocado Month" } ,
   {MAY, 12, "National Nutty Fudge Day" } ,
   {MAY, 13, "National Fruit Cocktail Day" } ,
   {MAY, 14, "National Buttermilk Biscuit Day" } ,
   {MAY, 15, "National Chocolate Chip Day" } ,
   {MAY, 16, "Celebrate National Asparagus Month" } ,
   {MAY, 17, "National Cherry Cobbler Day" } ,
   {MAY, 18, "National Cheese Souffle Day" } ,
   {MAY, 19, "National Devil's Food Cake Day" } ,
   {MAY, 20, "National Quiche Lorraine Day" } ,
   {MAY, 21, "National Strawberries and Cream Day" } ,
   {MAY, 22, "National Vanilla Pudding Day" } ,
   {MAY, 23, "National Taffy Day" } ,
   {MAY, 24, "National Escargot Day" } ,
   {MAY, 25, "Celebrate National Egg Month" } ,
   {MAY, 26, "National Blueberry Cheesecake Day" } ,
   {MAY, 27, "National Grape Popsicle Day" } ,
   {MAY, 28, "National Brisket Day" } ,
   {MAY, 29, "Celebrate National Salad Month" } ,
   {MAY, 30, "Celebrate National Barbeque Month" } ,
   {MAY, 31, "National Macaroon Day" } ,
   {JUN, 1, "National Hazelnut Cake Day" } ,
   {JUN, 2, "National Rocky Road Ice Cream Day" } ,
   {JUN, 3, "National Egg Day" } ,
   {JUN, 4, "National Frozen Yogurt Day" } ,
   {JUN, 5, "National Gingerbread Day" } ,
   {JUN, 6, "National Applesauce Cake Day" } ,
   {JUN, 7, "National Chocolate Ice Cream Day" } ,
   {JUN, 8, "National Jelly-filled Doughnut Day" } ,
   {JUN, 9, "National Strawberry Rhubarb Pie Day" } ,
   {JUN, 10, "National Herbs and Spice Day" } ,
   {JUN, 11, "National German Chocolate Cake Day" } ,
   {JUN, 12, "National Peanut Butter Cookie Day" } ,
   {JUN, 13, "Celebrate National Papaya Month" } ,
   {JUN, 14, "National Strawberry Shortcake Day" } ,
   {JUN, 15, "National Lobster Day" } ,
   {JUN, 16, "National Fudge Day" } ,
   {JUN, 17, "National Apple Strudel Day" } ,
   {JUN, 18, "National Cherry Tart Day" } ,
   {JUN, 19, "Celebrate National Dairy Month" } ,
   {JUN, 20, "National Vanilla Milkshake Day" } ,
   {JUN, 21, "National Peaches and Cream Day" } ,
   {JUN, 22, "National Onion Rings Day" } ,
   {JUN, 23, "National Pecan Sandy Day" } ,
   {JUN, 24, "National Pralines Day" } ,
   {JUN, 25, "National Strawberry Parfait Day" } ,
   {JUN, 26, "National Chocolate Pudding Day" } ,
   {JUN, 27, "National Orange Blossom Day" } ,
   {JUN, 28, "National Tapioca Day" } ,
   {JUN, 29, "National Almond Butter Crunch Day" } ,
   {JUN, 30, "National Ice Cream Soda Day" } ,
   {JUL, 1, "National Gingersnap Day" } ,
   {JUL, 2, "Celebrate National Hot Dog Month" } ,
   {JUL, 3, "Eat Beans Day" } ,
   {JUL, 4, "National Cesar Salad Day" } ,
   {JUL, 5, "National Apple Turnover Day" } ,
   {JUL, 6, "National Fried Chicken Day" } ,
   {JUL, 7, "National Strawberry Sundae Day" } ,
   {JUL, 8, "National Chocolate with Almonds Day" } ,
   {JUL, 9, "National Sugar Cookie Day" } ,
   {JUL, 10, "National Pina Colada Day" } ,
   {JUL, 11, "National Blueberry Muffin Day" } ,
   {JUL, 12, "National Pecan Pie Day" } ,
   {JUL, 13, "National French Fries Day" } ,
   {JUL, 14, "National Macaroni Day" } ,
   {JUL, 15, "National Tapioca Pudding Day" } ,
   {JUL, 16, "National Corn Fritters Day" } ,
   {JUL, 17, "National Peach Ice Cream Day" } ,
   {JUL, 18, "National Caviar Day" } ,
   {JUL, 19, "Celebrate Ice Cream Month" } ,
   {JUL, 20, "National Lollipop Day" } ,
   {JUL, 21, "National Junk Food Day" } ,
   {JUL, 22, "National Penuche Fudge Day" } ,
   {JUL, 23, "National Vanilla Ice Cream Day" } ,
   {JUL, 24, "Celebrate National Baked Bean Month" } ,
   {JUL, 25, "National Hot Fudge Sundae Day" } ,
   {JUL, 26, "Celebrate National Hotdog Month" } ,
   {JUL, 27, "National Creme Brule Day" } ,
   {JUL, 28, "National Milk Chocolate Day" } ,
   {JUL, 29, "Cheese Sacrifice Purchase Day" } ,
   {JUL, 30, "National Cheese Cake Day" } ,
   {JUL, 31, "Jump For Jelly Beans Day" } ,
   {AUG, 1, "National Raspberry Cream Pie Day" } ,
   {AUG, 2, "National Ice Cream Sandwich Day" } ,
   {AUG, 3, "National Watermelon Day" } ,
   {AUG, 4, "National Lasagna Day" } ,
   {AUG, 5, "National Waffle Day" } ,
   {AUG, 6, "National Root Bear Float Day" } ,
   {AUG, 7, "National Raspberries in Cream Day" } ,
   {AUG, 8, "National Zucchini Day" } ,
   {AUG, 9, "National Rice Pudding Day" } ,
   {AUG, 10, "National S'mores Day" } ,
   {AUG, 11, "National Raspberry Tart Day" } ,
   {AUG, 12, "National Julienne Fries Day" } ,
   {AUG, 13, "National Filet Mignon Day" } ,
   {AUG, 14, "National Creamsicle Day" } ,
   {AUG, 15, "National Lemon Meringue Pie Day" } ,
   {AUG, 16, "National Bratwurst Day" } ,
   {AUG, 17, "National Vanilla Custard Day" } ,
   {AUG, 18, "National Soft Ice Cream Day" } ,
   {AUG, 19, "National Potato Day" } ,
   {AUG, 20, "National Lemonade Day" } ,
   {AUG, 21, "National Pecan Torte Day" } ,
   {AUG, 22, "National Eat a Peach Day" } ,
   {AUG, 23, "National Sponge Cake Day" } ,
   {AUG, 24, "National Peach Pie Day" } ,
   {AUG, 25, "National Spumoni Day" } ,
   {AUG, 26, "National Cherry Popsicle Day" } ,
   {AUG, 27, "National Banana Lovers Day" } ,
   {AUG, 28, "National Cherry Turnover Day" } ,
   {AUG, 29, "National More Herbs, Less Salt Day" } ,
   {AUG, 30, "National Toasted Marshmallow Day" } ,
   {AUG, 31, "National Trail Mix Day" } ,
   {SEP, 1, "National Cherry Popover Day" } ,
   {SEP, 2, "National Blueberry Popsicle Day" } ,
   {SEP, 3, "Celebrate National Potato Month" } ,
   {SEP, 4, "National Macadamia Nut Day" } ,
   {SEP, 5, "National Cheese Pizza Day" } ,
   {SEP, 6, "Celebrate National Rice Month" } ,
   {SEP, 7, "National Acorn Squash Day" } ,
   {SEP, 8, "National Date Nut Bread Day" } ,
   {SEP, 9, "National Wiener Schnitzel Day" } ,
   {SEP, 10, "National TV dinner Day" } ,
   {SEP, 11, "National Hot Cross Bun Day" } ,
   {SEP, 12, "National Chocolate Milkshake Day" } ,
   {SEP, 13, "National Peanut Day" } ,
   {SEP, 14, "National Cream Filled Donut Day" } ,
   {SEP, 15, "National Linguini Day" } ,
   {SEP, 16, "National Guacamole Day" } ,
   {SEP, 17, "National Apple Dumpling Day" } ,
   {SEP, 18, "National Cheeseburger Day" } ,
   {SEP, 19, "National Butterscotch Pudding Day" } ,
   {SEP, 20, "Celebrate National Biscuit Month" } ,
   {SEP, 21, "National Pecan Cookie Day" } ,
   {SEP, 22, "National Ice Cream Cone Day" } ,
   {SEP, 23, "National White Chocolate Day" } ,
   {SEP, 24, "National Cherries Jubilee Day" } ,
   {SEP, 25, "National Crabmeat Newburg Day" } ,
   {SEP, 26, "National Pancake Day" } ,
   {SEP, 27, "National Chocolate Milk Day" } ,
   {SEP, 28, "National Strawberry Cream Pie Day" } ,
   {SEP, 29, "National Linguini Day" } ,
   {SEP, 30, "Celebrate National Chicken Month" } ,
   {SEP ,30, "National Chewing Gum Day" } ,
   {OCT, 1, "World Vegetarian Day" } ,
   {OCT, 2, "National Fried Scallops Day" } ,
   {OCT, 3, "National Caramel Custard Day" } ,
   {OCT, 4, "National Taco Day" } ,
   {OCT, 5, "National Apple Betty Day" } ,
   {OCT, 6, "National Noodle Day" } ,
   {OCT, 7, "National Frappe Day" } ,
   {OCT, 8, "Celebrate National Chili Month" } ,
   {OCT, 9, "Celebrate National Apple Month" } ,
   {OCT, 10, "National Angel Food Cake Day" } ,
   {OCT, 11, "World Egg Day" } ,
   {OCT, 12, "Celebrate National Popcorn Popping Month" } ,
   {OCT, 13, "National Yorkshire Pudding Day" } ,
   {OCT, 14, "Celebrate National Pretzel Month" } ,
   {OCT, 15, "National Chicken Cacciatore Day" } ,
   {OCT, 16, "Celebrate National Seafood Month" } ,
   {OCT, 17, "National Pasta Day" } ,
   {OCT, 18, "National Chocolate Cupcake Day" } ,
   {OCT, 19, "National Seafood Bisque Day" } ,
   {OCT, 20, "Celebrate National Pork Month" } ,
   {OCT, 21, "National Pumpkin Cheesecake Day" } ,
   {OCT, 22, "National Nut Day" } ,
   {OCT, 23, "National Boston Cream Pie Day" } ,
   {OCT, 24, "National Bologna Day" } ,
   {OCT, 25, "National Greasy Foods Day" } ,
   {OCT, 26, "National Mincemeat Pie Day" } ,
   {OCT, 27, "National Potato Day" } ,
   {OCT, 28, "National Chocolate Day" } ,
   {OCT, 29, "National Oatmeal Day" } ,
   {OCT, 30, "National Candy Corn Day" } ,
   {OCT, 31, "National Candy Apple Day" } ,
   {NOV, 1, "National Vinegar Day" } ,
   {NOV, 2, "National Deviled Egg Day" } ,
   {NOV, 3, "National Sandwich Day" } ,
   {NOV, 4, "National Candy Day" } ,
   {NOV, 5, "National Doughnut Day" } ,
   {NOV, 6, "National Nachos Day" } ,
   {NOV, 7, "National Bittersweet Chocolate with Almonds Day" } ,
   {NOV, 8, "Celebrate National Raisin Bread Day" } ,
   {NOV, 9, "Celebrate National Vegan Month" } ,
   {NOV, 10, "National Vanilla Cupcake Day" } ,
   {NOV, 11, "National Sundae Day" } ,
   {NOV, 12, "National Chicken Soup for the Soul Day" } ,
   {NOV, 13, "National Indian Pudding Day" } ,
   {NOV, 14, "National Pickle Day" } ,
   {NOV, 15, "National Raisin Bran Cereal Day" } ,
   {NOV, 16, "Celebrate National Peanut Butter Lover's Month" } ,
   {NOV, 17, "National Homemade Bread Day" } ,
   {NOV, 18, "Celebrate National Georgia Pecan Month" } ,
   {NOV, 19, "Celebrate National Pepper Month" } ,
   {NOV, 20, "National Peanut Butter Fudge Day" } ,
   {NOV, 21, "National Gingerbread Day" } ,
   {NOV, 22, "National Cashew Day" } ,
   {NOV, 23, "National Eat a Cranberry Day" } ,
   {NOV, 24, "National Sardines Day" } ,
   {NOV, 25, "National Parfait Day" } ,
   {NOV, 26, "National Cake Day" } ,
   {NOV, 27, "National Bavarian Cream Pie Day" } ,
   {NOV, 28, "National French Toast Day" } ,
   {NOV, 29, "National Lemon Cream Pie Day" } ,
   {NOV, 30, "National Mousse Day" } ,
   {DEC, 1, "National Eat a Red Apple Day" } ,
   {DEC, 2, "National Fritters Day" } ,
   {DEC, 3, "National Apple Pie Day" } ,
   {DEC, 4, "National Cookie Day" } ,
   {DEC, 5, "National Sachertorte Day" } ,
   {DEC, 6, "National Microwave Oven Day" } ,
   {DEC, 8, "National Chocolate Brownie Day" } ,
   {DEC, 9, "National Pastry Day" } ,
   {DEC, 10, "Celebrate National Fruit Cake Month" } ,
   {DEC, 11, "National Noodle Ring Day" } ,
   {DEC, 12, "National Cocoa Day" } ,
   {DEC, 13, "National Ice Cream and Violins Day" } ,
   {DEC, 14, "National Bouillabaisse Day" } ,
   {DEC, 15, "National Cupcake Day" } ,
   {DEC, 16, "National Chocolate Covered Anything Day" } ,
   {DEC, 17, "National Maple Syrup Day" } ,
   {DEC, 18, "National Roast Pig Day" } ,
   {DEC, 19, "National Hard Candy Day" } ,
   {DEC, 20, "National Fried Shrimp Day" } ,
   {DEC, 21, "National Kiwi Fruit Day" } ,
   {DEC, 22, "National Date Nut Bread Day" } ,
   {DEC, 23, "National Pfeffernuesse Day" } ,
   {DEC, 24, "National Egg Nog Day" } ,
   {DEC, 25, "National Pumpkin Pie Day" } ,
   {DEC, 26, "National Candy Cane Day" } ,
   {DEC, 27, "National Fruit Cake Day" } ,
   {DEC, 28, "National Chocolate Candy Day" } ,
   {DEC, 29, "National Pepper Pot Day" } ,
   {DEC, 30, "National Bicarbonate of Soda Day" } ,
   {DEC, 31, "Celebrate National Egg Nog Month" } ,

 {0,0,NULL} } ;  /* the last element, a flag to stop searching */

/*---------------- Dates of Easter thru the years ---------------*/

typedef struct { int yy,mm,dd; } yymmdd ;

static yymmdd EasterDate[] = {
   {2011,4,24} , {2012,4,8}  , {2013,3,31} , {2014,4,20} , {2015,4,5}  ,
   {2016,3,27} , {2017,4,16} , {2018,4,1}  , {2019,4,21} , {2020,4,12} ,
   {2021,4,4}  , {2022,4,17} , {2023,4,9}  , {2024,3,31} , {2025,4,20} ,
   {2026,4,5}  , {2027,3,28} , {2028,4,16} , {2029,4,1}  , {2030,4,21} ,
   {2031,4,13} , {2032,3,28} , {2033,4,17} , {2034,4,9}  , {2035,3,25} ,
   {2036,4,13} , {2037,4,5}  , {2038,4,25} , {2039,4,10} , {2040,4,1}  ,
   {2041,4,21} , {2042,4,6}  , {2043,3,29} , {2044,4,17} , {2045,4,9}  ,
   {2046,3,25} , {2047,4,14} , {2048,4,5}  , {2049,4,18} , {2050,4,10} ,
   {2051,4,2}  , {2052,4,21} , {2053,4,6}  , {2054,3,29} , {2055,4,18} ,
   {2056,4,2}  , {2057,4,22} , {2058,4,14} , {2059,3,30} , {2060,4,18} ,
   {2061,4,10} , {2062,3,26} , {2063,4,15} , {2064,4,6}  , {2065,3,29} ,
   {2066,4,11} , {2067,4,3}  , {2068,4,22} , {2069,4,14} , {2070,3,30} ,
   {2071,4,19} , {2072,4,10} , {2073,3,26} , {2074,4,15} , {2075,4,7}  ,
   {2076,4,19} , {2077,4,11} , {2078,4,3}  , {2079,4,23} , {2080,4,7}  ,
   {2081,3,30} , {2082,4,19} , {2083,4,4}  , {2084,3,26} , {2085,4,15} ,
   {2086,3,31} , {2087,4,20} , {2088,4,11} , {2089,4,3}  , {2090,4,16} ,
   {2091,4,8}  , {2092,3,30} , {2093,4,12} , {2094,4,4}  , {2095,4,24} ,
   {2096,4,15} , {2097,3,31} , {2098,4,20} , {2099,4,12} , {2100,3,28} ,
   {2101,4,17} , {2102,4,9}  , {2103,3,25} , {2104,4,13} , {2105,4,5}  ,
   {2106,4,18} , {2107,4,10} , {2108,4,1}  , {2109,4,21} , {2110,4,6}  ,
   {2111,3,29} , {2112,4,17} , {2113,4,2}  , {2114,4,22} , {2115,4,14} ,
   {2116,3,29} , {2117,4,18} , {2118,4,10} , {2119,3,26} , {2120,4,14} ,
   {2121,4,6}  , {2122,3,29} , {2123,4,11}
} ;

#define EASTER_FIRST 2011
#define EASTER_LAST  2123  /* if AFNI lasts this long -- wow! */

int AFNI_is_Easter( int yy , int mm , int dd )
{
   if( yy < EASTER_FIRST || yy > EASTER_LAST ) return 0 ;
   yy = yy - EASTER_FIRST ;
   return ( mm == EasterDate[yy].mm && dd == EasterDate[yy].dd ) ;
}

/*----------------------------------------------------------------------------*/
/* Stuff for Rosh Hashanah calculation [22 Sep 2017] */

/* Simple day of week calculator (Gregorian):
   m=1..12 ; d=1..31 ; y=1800... ; return=0..6 (Sun..Sat) */

static int dow(int m,int d,int y){
  y -= (m<3) ;
  return(y+y/4-y/100+y/400+"-bed=pen+mad."[m]+d)%7;
}

/* Compute date of Rosh Hashanah in Sep, given year number (>= 1800):
   see https://quasar.as.utexas.edu/BillInfo/ReligiousCalendars.html */

static int rosh(int y)
{
   int gold , nn , dd ;
   double nplus , ff ;

   gold = y%19 + 1 ; /* G */

   /* N + fraction =
       {[Y/100] - [Y/400] - 2} + 765433/492480*Remainder(12G|19)
                               + Remainder(Y|4)/4 - (313Y+89081)/98496 */

   nplus = ((y/100)-(y/400)-2) + 765433.0/492480.0*((12*gold)%19)
                               + 0.25*(y%4) - (313.0*y+89081.0)/98496.0 ;

   nn = (int)nplus ;
   ff = nplus - (double)nn ;

   dd = dow(9,nn,y) ; /* day of week: 0..6 (Sun..Sat) */

   /* If the day calculated above is a Sunday, Wednesday, or Friday,
      Rosh Hashanah falls on the next day (Monday, Thursday or Saturday).

      If the calculated day is a Monday, and if the fraction is greater than
      or equal to 23269/25920, and if Remainder(12G|19) is greater than 11,
      Rosh Hashanah falls on the next day, a Tuesday.

      If it is a Tuesday, and if the fraction is greater than or equal to
      1367/2160, and if Remainder(12G|19) is greater than 6, Rosh Hashanah
      falls two days later, on Thursday (NOT WEDNESDAY!!).                 */

   if( dd==0 || dd==3 || dd==5 ){                    /* Sun, Wed, or Fri are taboo */
     nn++ ;
   } else if( dd==1 && ff >= 23269.0/25920.0 && (12*gold)%19 > 11 ){ /* Mon -> Tue */
     nn++ ;
   } else if( dd==2 && ff >= 1367.0/2160.0   && (12*gold)%19 >  6 ){ /* Tue -> Thu */
     nn += 2 ;
   }

   return nn ;
}

int AFNI_is_Rosh_Hashanah( int yy , int mm , int dd )
{
   return ( (mm==9) && (rosh(yy)==dd) ) ;
}

/*------------------------------------------------------------------------------*/

static yymmdd DiwaliDate[] = {
 {2011,10,26} , {2012,11,13} , {2013,11,3}  , {2014,10,23} , {2015,11,11} ,
 {2016,10,30} , {2017,10,19} , {2018,11,7}  , {2019,10,27} , {2020,11,14} ,
 {2021,11,4}  , {2022,10,24}
} ;

#define DIWALI_FIRST 2011
#define DIWALI_LAST  2022  /* I hope AFNI lasts until this year -- at least! */

int AFNI_is_Diwali( int yy , int mm , int dd )
{
   if( yy < DIWALI_FIRST || yy > DIWALI_LAST ) return 0 ;
   yy = yy - DIWALI_FIRST ;
   return ( mm == DiwaliDate[yy].mm && dd == DiwaliDate[yy].dd ) ;
}

/*------------------------------------------------------------------------------*/
typedef struct { int mm,dd,yy; } mmddyy ;

static mmddyy YomKippurDate[] = {
  {OCT,4,2014}, {SEP,23,2015}, {OCT,12,2016}, {SEP,30,2017},
  {SEP,19,2018}, {OCT,9,2019}, {SEP,28,2020}, {SEP,16,2021},
  {OCT,5,2022}, {SEP,25,2023}, {OCT,12,2024}, {OCT,2,2025},
  {SEP,21,2026}, {OCT,11,2027}, {SEP,30,2028}, {SEP,19,2029},
  {OCT,7,2030}, {SEP,27,2031}, {SEP,15,2032}, {OCT,3,2033},
  {SEP,23,2034}, {OCT,13,2035}, {OCT,1,2036}, {SEP,19,2037},
  {OCT,9,2038}, {SEP,28,2039}, {SEP,17,2040}, {OCT,5,2041},
  {SEP,24,2042}, {OCT,14,2043}, {OCT,1,2044}, {SEP,21,2045},
  {OCT,10,2046}, {SEP,30,2047}, {SEP,17,2048}, {OCT,6,2049}
} ;

#define YOMKIPPUR_FIRST 2014
#define YOMKIPPUR_LAST  2049

int AFNI_is_YomKippur( int yy , int mm , int dd )
{
   if( yy < YOMKIPPUR_FIRST || yy > YOMKIPPUR_LAST ) return 0 ;
   yy = yy - YOMKIPPUR_FIRST ;
   return ( mm == YomKippurDate[yy].mm && dd == YomKippurDate[yy].dd ) ;
}

/*------------------------------------------------------------------------------*/

static mmddyy EidDate[] = {
  {JUL,29,2014}, {JUL,18,2015}, {JUL,7,2016}, {JUN,26,2017}, {JUN,15,2018},
  {JUN,5,2019}, {MAY,24,2020}, {MAY,13,2021}, {MAY,3,2022}, {APR,22,2023},
  {APR,10,2024}, {MAR,31,2025}, {MAR,20,2026}, {MAR,10,2027}, {FEB,27,2028},
  {FEB,15,2029}, {FEB,5,2030}, {JAN,25,2031}, {JAN,14,2032}, {JAN,3,2033},
  {DEC,23,2033}, {DEC,12,2034}, {DEC,2,2035}, {NOV,20,2036}, {NOV,10,2037},
  {OCT,30,2038}, {OCT,19,2039}, {OCT,8,2040}, {SEP,27,2041}, {SEP,16,2042},
  {SEP,6,2043}, {AUG,25,2044}, {AUG,15,2045}, {AUG,4,2046}, {JUL,24,2047},
  {JUL,13,2048}, {JUL,2,2049}
} ;

#define EID_FIRST 2014
#define EID_LAST  2049

int AFNI_is_Eid( int yy , int mm , int dd )
{
   if( yy < EID_FIRST || yy > EID_LAST ) return 0 ;
   yy = yy - EID_FIRST ;
   return ( mm == EidDate[yy].mm && dd == EidDate[yy].dd ) ;
}

/*------------------------------------------------------------------------------*/

char * julian_date_string(void)  /* 29 Oct 2018 */
{
   double tt ; int ii,jj ;
   static char jdate[128] ; char *ss ;

   tt = (double)time(NULL) ;   /* seconds since 01 Jan 1970 */
   tt = ( tt / 86400.0 ) + 2440587.5 ;
   ii = (int)tt ;
   ss = commaized_integer_string(ii) ;
   jj = (int)rint((tt-(double)ii)*100000.0) ;
   sprintf(jdate,"Julian Date: %s.%0d" , ss , jj ) ;
   return jdate ;
}

/*------------------------------------------------------------------------------*/
/*! Return one of today's date trivia string. */

char * AFNI_get_date_trivia(void)
{
   time_t tt ;
   struct tm *lt ;
   int ii ;
   static char *monthlist[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" } ;
   static char dmy[32] ;
   static int ncall=0 ;

   tt = time(NULL) ;         /* seconds since 01 Jan 1970 */
   lt = localtime( &tt ) ;   /* break into pieces */

   /**** find this month and day in the trivia list, if present ****/

   for( ii=ntar=0 ; ntar < NTMAX && holiday[ii].day != 0 ; ii++ )
     if( holiday[ii].mon == lt->tm_mon+1 && holiday[ii].day == lt->tm_mday )
       tar[ntar++] = holiday[ii].label ;

   /**** Special days not on fixed dates ****/

   /* Day of month = 15 and month = 2 */

   if( ntar < NTMAX && lt->tm_mday == 15 && lt->tm_mon+1 == FEB ){
     static char aniv[64] ; int yy = lt->tm_year-95 ;
     if( yy > 0 ){
       sprintf(aniv,"Anniversary of AFNI's release: #%d",yy) ;
       tar[ntar++] = aniv ;
     }
   }

   /* Day of month = 13 and weekday = Friday */

   if( ntar < NTMAX && lt->tm_mday == 13 && lt->tm_wday == 5 )
      tar[ntar++] = "Friday the 13th" ;

   /* 1st Monday in September */

   if( ntar < NTMAX && lt->tm_mon+1 == SEP && lt->tm_wday == 1 && lt->tm_mday <= 7 )
      tar[ntar++] = "Labor Day (USA)" ;

   /* 4th Thursday in November */

   if( ntar < NTMAX && lt->tm_mon+1 == NOV && lt->tm_wday == 4 &&
                       lt->tm_mday <= 28   && lt->tm_mday >= 22  )
      tar[ntar++] = "Thanksgiving (USA)" ;

   /* 1st Monday in October */

   if( ntar < NTMAX && lt->tm_mon+1 == OCT && lt->tm_wday == 1 && lt->tm_mday <= 7 )
      tar[ntar++] = "Opening of Supreme Court (USA)" ;

   /* 1st Tuesday after 1st Monday in November */

   if( ntar < NTMAX && lt->tm_mon+1 == NOV && lt->tm_wday == 2 &&
                       lt->tm_mday >= 2    && lt->tm_mday <= 8   )
      tar[ntar++] = "Election Day (USA)" ;

   /* Easter? */

   if( ntar < NTMAX && AFNI_is_Easter(lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday) )
      tar[ntar++] = "Easter (Western rite)" ;

   /* Rosh Hashanah [22 Sep 2017] */

   if( ntar < NTMAX && AFNI_is_Rosh_Hashanah(lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday) )
      tar[ntar++] = "Rosh Hashanah" ;

   /* Diwali? */

   if( ntar < NTMAX && AFNI_is_Diwali(lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday) )
      tar[ntar++] = "Diwali / Deepavali" ;

   /* Yom Kippur? */

   if( ntar < NTMAX && AFNI_is_YomKippur(lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday) )
      tar[ntar++] = "start of Yom Kippur" ;

   /* Eid? */

   if( ntar < NTMAX && AFNI_is_Eid(lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday) )
      tar[ntar++] = "Eid al-Fitr" ;

   if( ntar < NTMAX )
      tar[ntar++] = julian_date_string() ;

   ncall++ ;
   if( ntar == 0 || (ntar < NTMAX && ncall > 3) ){  /* 04 Oct 2007 */
     char *fm = AFNI_get_friend() ;
     tar[ntar++] = fm ;
   }

   /**** select which one to return ***/

   if( ntar == 1 ){
     return tar[0] ;
   } else if( ntar > 1 ){
     static int iold=-1 ;
     ii = (lrand48()>>8) % ntar ;
     if( ii == iold ) ii = (ii+1)%ntar ;
     iold = ii ; return tar[ii] ;
   }

   /* default trivia */

#if 1
   sprintf( dmy , "[%02d %s %d]" ,
           lt->tm_mday , monthlist[lt->tm_mon] , lt->tm_year+1900 ) ;
   return dmy ;
#else
   return "[Elen sila lumenn' omentielvo]" ;
#endif

}
