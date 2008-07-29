#pragma ident "$Id$"

/**
 * @file ConfDataReader.hpp
 * Class to parse and manage configuration data files.
 */

#ifndef CONFDATAREADER_HPP
#define CONFDATAREADER_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================




#include <string>
#include <map>
#include "FFTextStream.hpp"
#include "StringUtils.hpp"
#include "Matrix.hpp"


using namespace std;

namespace gpstk
{
      /// Thrown when there is a problem reading a configuration file
      /// @ingroup exceptiongroup
   NEW_EXCEPTION_CLASS(ConfFileException, gpstk::Exception);


      /** @addtogroup formattedfile */
      //@{

      /** This is a class to parse and manage configuration data files.
       *
       * A typical way to use this class follows:
       *
       * @code
       *
       *      // Declare a ConfDataReader object
       *   ConfDataReader confRead;
       *
       *      // Open and parse configuration file
       *   confRead.open("configuration.txt");
       *
       *      // Read variable 'name' from section 'ONSA' using '()' operator
       *   string rxName( confRead("name", "ONSA') );
       *
       *      // Get default tolerance from default section "DEFAULT"
       *   double tolerance;
       *   tolerance = confRead.getValueAsDouble("tolerance");
       *
       *      // Print "baseline" description, value and units
       *   cout << confRead.getVariableDescription("baseline") << endl;
       *   cout << confRead.getValueAsDouble("baseline")       << endl;
       *   cout << confRead.getValueDescription("baseline")    << endl;
       *
       *      // Read if receiver "BELL" will be treated as a reference rx
       *   bool bellRef( confRead.getValueAsBoolean("reference", "BELL") );
       *
       * @endcode
       *
       * ConfDataReader class provides powerful objects to read and manage
       * configuration data files. They support multiple sections, variable
       * descriptions, and value descriptions (such as units).
       *
       * An example of configuration file follows:
       *
       * @code
       *
       * # This is a configuration file, and this is a comment line
       * ; This is also a comment line
       *
       * # The general format is:
       * ; variableName, variableComment = value, valueComment
       *
       * # 'variableComment' and 'valueComment' are optional
       *
       * # So far, there is no section declared, so the following couple of
       * # variables are stored in section "DEFAULT"
       *
       * baseline, baseline between receivers = 13.434510, kilometers
       * tolerance, allowed difference between time stamps = 1.5, secs
       * reportExceptions = TRUE    ; Recover this with 'getValueAsBoolean()'
       *
       * # Declare a section
       * [ONSA]
       *
       * name, 4-char station name = ONSA
       *
       * staX, X station coordinate = 3370658.5419, meters
       * staY, Y station coordinate =  711877.1496, meters
       * staZ, Z station coordinate = 5349786.9542, meters
       *
       * antennaType : AOAD/M_B   # Note that you can use ':' instead of '='
       *
       *
       * [BELL]
       * reference = TRUE
       *
       *
       * [ROVER]
       * speed = 0.223 , m/s
       *
       *
       * [ONSA]   ; you may reuse a previous section and add new variables
       *          ; without problems (but with different names!!!)
       *
       * sampling, sampling period = 30, s
       *
       * @endcode
       *
       * The configuration file follows the following format:
       *
       * - Anything after a '#' or a ';' is a comment
       * - Blank lines are ignored.
       * - No line may have more than 255 characters.
       * - Variable and section names are ALWAYS converted to uppercase.
       * - Variable and section names MUST start with a letter, and must only
       *   contain a mix of letters, numbers, dashes (-) and underscores (_).
       * - Section names must be enclosed in brackets ([]).
       * - The Variable/Value pairs are separated by either '=' or ':'.
       * - You may add comments to variables and values. Use a comma to
       *   separate such comments.
       * - If you use the same variable name within a given section, only the
       *   last value will be used. Remember that names are ALWAYS converted to
       *   uppercase.
       *
       * By default, values are returned as std::string's, but there are methods
       * available to get them as booleans, integers and doubles. Comments are
       * always returned as std::string's.
       *
       * This class is very strict and throws a 'ConfFileException' exception
       * when trying to access variables that don't exist in the configuration
       * file. This behaviour may be changed with the 'setIssueException()'
       * method.
       *
       * The format of the Variable/Value pairs is inspired in the options file
       * used in the GNSSTk project. Thanks folks!.
       */
   class ConfDataReader : public FFTextStream
   {
   public:

         /// Default constructor
      ConfDataReader() : issueException(true) {};


         /** Common constructor. It will always open 'file' for read and will
          *  configuration data in one pass.
          *
          * @param file    Configuration data file to read
          *
          */
      ConfDataReader(const char* file)
         : FFTextStream(file, std::ios::in), issueException(true)
      { loadData(); };


         /** Common constructor. It will always open 'fn' for read and will
          *  configuration data in one pass.
          *
          * @param file    Configuration data file to read
          *
          */
      ConfDataReader(const string& file)
         : FFTextStream(file.c_str(), std::ios::in), issueException(true)
      { loadData(); };


         /// Method to open AND load configuration data file.
      virtual void open(const char* fn);


         /// Method to open AND load configuration data file.
      virtual void open(const string& fn);


         /** Method to get the value of a given variable as a string
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual string getValue( string variable,
                               string section = "DEFAULT" )
         throw(ConfFileException);


         /** Method to get the value of a given variable as a double
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual double getValueAsDouble( string variable,
                                       string section = "DEFAULT" )
         throw(ConfFileException)
      { return StringUtils::asDouble( getValue(variable, section) ); };


         /** Method to get the value of a given variable as an integer
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual int getValueAsInt( string variable,
                                 string section = "DEFAULT" )
         throw(ConfFileException)
      { return StringUtils::asInt( getValue(variable, section) ); };


         /** Method to get the value of a given variable as a boolean
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual bool getValueAsBoolean( string variable,
                                      string section = "DEFAULT" )
         throw(ConfFileException);


         /** Method to get the description of a given variable
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual string getVariableDescription( string variable,
                                             string section = "DEFAULT" )
         throw(ConfFileException);


         /** Method to get the description of a given value
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual string getValueDescription( string variable,
                                          string section = "DEFAULT" )
         throw(ConfFileException);


         /** Method to set whether an exception will be issued
          * when requesting an invalid variable (or section), or not.
          *
          * @param issueEx    Whether an exception will be issued or not
          *
          */
      ConfDataReader& setIssueException(bool issueEx)
      { issueException = issueEx; return (*this); }


         /// Method to clear the stored variables.
      virtual ConfDataReader& clear(void)
      { confData.clear(); return (*this); };


         /// Method to get the name of each section in order.
      virtual string getEachSection(void);


         /// Method to reset the iterator traversing section names. This method
         /// is intended to be used complementing method 'getEachSection()'.
      virtual void resetSection(void)
      { itCurrentSection = confData.begin(); return; };


         /** Operator to get the value of a given variable as a string
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual string operator()( string variable,
                                 string section = "DEFAULT" )
         throw(ConfFileException)
      { return getValue(variable, section); };


         /// Destructor
      virtual ~ConfDataReader() {}


   private:


         /// This boolean field determines whether an exception will be issued
         /// when requesting an invalid variable (or section), or not
      bool issueException;


         /// A structure used to store variable's data.
      struct variableData
      {
            // Default constructor initializing the data in the structure
         variableData() : varComment(""), value(""), valueComment("") {};

         string varComment;      ///< Variable comment
         string value;           ///< Value of variable
         string valueComment;    ///< Value comment
      };


         /// Define 'variableMap' type
      typedef std::map<string, variableData> variableMap;

         /// Define 'confMap' type
      typedef std::map<string, variableMap> confMap;



         /// Map holding the configuration information
      confMap confData;


         /// Iterator pointing to the current section
      confMap::const_iterator itCurrentSection;


         /** Method to check if the given parameter name is properly formed.
          *
          * @param name    Name to the checked.
          */
      virtual bool checkName(string name);


         /** Method to check if a given section/variable pair exists.
          *
          * @param variable   Variable name.
          * @param section    Section the variable belongs to.
          *
          */
      virtual bool ifExist( string variable,
                            string section )
         throw(ConfFileException);


         /// Method to store conf data in this class' data map
      virtual void loadData(void)
         throw(ConfFileException);


   };

      //@}

} // namespace
#endif  // CONFDATAREADER_HPP