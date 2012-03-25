/* This file has been prepared for Doxygen automatic documentation generation.*/
/*
 * Copyright (C) 2012 Yuriy Kulikov
 *      Universitaet Erlangen-Nuernberg
 *      LS Informationstechnik (Kommunikationselektronik)
 *      Support email: Yuriy.Kulikov.87@googlemail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * strings.h
 *
 * Contains strings, which have to be stored in the program space
 * (flash memory), in order to save precious RAM. Strings have to be
 * read using special functions found in <avr/pgmspace.h>
 *
 * See http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
 */

#ifndef STARINGS_H_
#define STARINGS_H_

// Common
extern char Strings_newline[];
extern char Strings_colon[];
extern char Strings_space[];
extern char Strings_comma[];
extern char Strings_hex[];
// Spi task
extern char Strings_SpiMasterExample1[];
extern char Strings_SpiMasterExample2[];
extern char Strings_SpiMasterExample3[];
extern char Strings_SpiSlaveExample1[];

extern char Strings_SpiExampleCmd[];
extern char Strings_SpiExampleCmdDesc[];
extern char Strings_SpiExampleCmdResp[];
// CommandInterpreterTask
extern char Strings_BlinkCmd[];
extern char Strings_BlinkCmdDesc[];
extern char Strings_BlinkResp[];
extern char Strings_BlinkCmdError[];
// CommandInterpreter
extern char Strings_InterpretorError[];
extern char Strings_HelpCmd[];
extern char Strings_HelpCmdDesc[];

#endif /* STARINGS_H_ */
