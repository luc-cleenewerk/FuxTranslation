#ifndef UTILITIES
#define UTILITIES

#include <gecode/int.hh>
#include <gecode/minimodel.hh>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Gecode;



/***********************************************************************************************************************
 *                                                                                                                     *
 *                                                   Useful classes                                                    *
 *                                                                                                                     *
 ***********************************************************************************************************************/


class PartClass: public Space{
    public:

        //CONSTANT VARIABLES
        //Major triad (0,4,7)
        const IntSet MAJ_H_TRIAD = IntSet({0,4,7});
        //H triad (0,4,7)
        const IntSet H_TRIAD = IntSet({0,4,7});
        //PERFECT CONSONANCES (0,7)
        const IntVarArgs P_CONS = IntVarArgs({IntVar(*this,0,0), IntVar(*this,7,7)});
        //IMPERFECT CONSONANCES (3,4,8,9)
        const IntSet IMP_CONS = IntSet({3,4,8,9});
        //ALL CONSONANCES (0,3,4,7,8,9)
        const IntSet ALL_CONS = IntSet({0,3,4,7,8,9});
        const IntVarArgs ALL_CONS_VAR = IntVarArgs({IntVar(*this, 0, 0), IntVar(*this, 3,3), IntVar(*this, 4,4), IntVar(*this, 7, 7),
        IntVar(*this, 8,8),IntVar(*this, 9,9)});

        int species; //0 for cf, 1 for 1st, 2 for 2nd, 3 for 3rd, 4 for 4th, 5 for 5th
        IntVarArray solution_array;
        int solution_len;

        //voice variables
        vector<int> cp_range;
        vector<int> cp_domain;
        vector<int> chromatic_cp_domain;
        vector<int> extended_cp_domain;
        vector<int> off_domain;
        int voice_type;

        //1st species variables
        vector<IntVarArray> notes;
        vector<IntVarArray> h_intervals;
        vector<IntVarArray> m_intervals_brut;
        IntVarArray h_intervals_brut;
        vector<int> m_intervals;
        vector<int> motions;
        vector<int> motions_cost;
        vector<BoolVarArray> is_cf_lower_arr;
        vector<int> m2_intervals_brut;
        vector<int> m2_intervals;
        //some variables still not there, they'll come when we extend the code

        //3 voices variables
    
    public:
        PartClass(int n_measures, int species);

        void init_m_intervals_brut(int n_measures);

        vector<IntVarArray> get_notes();

        PartClass(PartClass &s);

        virtual Space *copy(void);

        void set_notes(int n);
};






/***********************************************************************************************************************
 *                                                                                                                     *
 *                                                  Useful constants                                                   *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/** Files */
const string LOG_FILE = "log.txt";
const string STATISTICS_FILE = "statistics.txt";
const string STATISTICS_CSV = "statistics";

/** Types of search engines */
enum solver_types{
    DFS_SOLVER, //0
    BAB_SOLVER, //1
    LDS_SOLVER  //2
};

/** Branching strategies */
enum variable_selection{
    DEGREE_MAX,                     //0
    DOM_SIZE_MIN,                   //1
    RIGHT_TO_LEFT,                  //2
    LEFT_TO_RIGHT_SOPRANO_TO_BASS,  //3
    AFC_MAX,                        //4
};

// /// go <-- soprano->bass: 4-3-2-1-8-7-6-5 etc
// auto right_to_left = [](const Space& home, const IntVar& x, int i) {
//     return i;
// };

// /// go --> soprano->bass
// auto left_to_right_soprano_to_bass = [](const Space& home, const IntVar& x, int i) {
//     return (i/4) * 4 + (4 - i%4);
// };

// const vector<IntVarBranch> variable_selection_heuristics = {INT_VAR_DEGREE_MAX(),
//                                                             INT_VAR_SIZE_MIN(),
//                                                             INT_VAR_MERIT_MAX(right_to_left),
//                                                             INT_VAR_MERIT_MIN(left_to_right_soprano_to_bass)};

const vector<string> variable_selection_heuristics_names = {"Degree max", "Domain size min", "Left to right",
                                                            "Right to left", "AFC max"};

enum value_selection{
    VAL_MIN,            //0
    VAL_MAX,            //1
    VAL_MED,            //2
    VAL_RND,            //3
};

// /// value selection heuristic
// auto branchVal = [](const Space& home, IntVar x, int i) {
//     return x.min();
// };

// /// commit function (EQ and DIFF)
// auto branchCommit = [](Space& home, unsigned int a, IntVar x, int i, int n){
//     if (a == 0U){
//         rel(home, x, IRT_EQ, n);
//     } else {
//         rel(home, x, IRT_NQ, n);
//     }
// };

const vector<IntValBranch> value_selection_heuristics = {INT_VAL_MIN(), INT_VAL_MAX(), INT_VAL_MED(), INT_VAL_RND(1U)};

const vector<string> value_selection_heuristics_names = {"Value min", "Value max", "Median value", "Value random"};

/** Voice ranges */
const int BASS_MIN = 40;
const int BASS_MAX = 60;
const int TENOR_MIN = 48;
const int TENOR_MAX = 69;
const int ALTO_MIN = 55;
const int ALTO_MAX = 75;
const int SOPRANO_MIN = 60;
const int SOPRANO_MAX = 84;

/** Melodic costs */
const int UNISON_COST = 0;
const int SECOND_COST = 1;
const int THIRD_COST = 3;
const int FOURTH_COST = 6;
const int FIFTH_COST = 6;
const int SIXTH_COST = 12;
const int SEVENTH_COST = 18;
const int OCTAVE_COST = 6;

const int MAX_MELODIC_COST = SEVENTH_COST;


/** Notes */
const int B_SHARP = 0;
const int C = 0;
const int C_SHARP = 1;
const int D_FLAT = 1;
const int D = 2;
const int D_SHARP = 3;
const int E_FLAT = 3;
const int E = 4;
const int F_FLAT = 4;
const int E_SHARP = 5;
const int F = 5;
const int F_SHARP = 6;
const int G_FLAT = 6;
const int G = 7;
const int G_SHARP = 8;
const int A_FLAT = 8;
const int A = 9;
const int A_SHARP = 10;
const int B_FLAT = 10;
const int B = 11;
const int C_FLAT = 11;

const vector<std::string> noteNames = {"C", "Csharp", "D", "Eb", "E", "F", "Fsharp", "G", "Ab", "A", "Bb", "B"};

/** Voice positions */
//enum voices{
//    BASS,       //0
//    TENOR,      //1
//    ALTO,       //2
//    SOPRANO     //3
//};

const vector<std::string> voiceNames = {"Bass", "Tenor", "Alto", "Soprano"};

/** scale degrees */
enum degrees{
    FIRST_DEGREE,       //0
    SECOND_DEGREE,      //1
    THIRD_DEGREE,       //2
    FOURTH_DEGREE,      //3
    FIFTH_DEGREE,       //4
    SIXTH_DEGREE,       //5
    SEVENTH_DEGREE      //6
};

const vector<std::string> degreeNames = {"First degree", "Second degree", "Third degree", "Fourth degree", "Fifth degree",
                                         "Sixth degree", "Seventh degree"};

/** Intervals */
// "classic" intervals
enum intervals{
    UNISSON,            //0
    MINOR_SECOND,       //1
    MAJOR_SECOND,       //2
    MINOR_THIRD,        //3
    MAJOR_THIRD,        //4
    PERFECT_FOURTH,     //5
    TRITONE,            //6
    PERFECT_FIFTH,      //7
    MINOR_SIXTH,        //8
    MAJOR_SIXTH,        //9
    MINOR_SEVENTH,      //10
    MAJOR_SEVENTH,      //11
    PERFECT_OCTAVE      //12
};

// augmented/diminished intervals
const int AUGMENTED_SECOND = 3;
const int AUGMENTED_FOURTH = TRITONE;
const int DIMINISHED_FIFTH = TRITONE;

/** Chords */
enum chordTypes{
    MAJOR_CHORD,                //0
    MINOR_CHORD,                //1
    DIMINISHED_CHORD,           //2
    AUGMENTED_CHORD,            //3
    DOMINANT_SEVENTH_CHORD,     //4
    MAJOR_SEVENTH_CHORD,        //5
    MINOR_SEVENTH_CHORD         //6
};

/// Types of chords represented by the intervals between their notes in root position up to an octave
const vector<int> MAJOR_CHORD_INTERVALS = {MAJOR_THIRD, MINOR_THIRD, PERFECT_FOURTH};
const vector<int> MINOR_CHORD_INTERVALS = {MINOR_THIRD, MAJOR_THIRD, PERFECT_FOURTH};
const vector<int> DIMINISHED_CHORD_INTERVALS = {MINOR_THIRD, MINOR_THIRD, TRITONE};
const vector<int> AUGMENTED_CHORD_INTERVALS = {MAJOR_THIRD, MAJOR_THIRD, MAJOR_THIRD};
const vector<int> DOMINANT_SEVENTH_CHORD_INTERVALS = {MAJOR_THIRD, MINOR_THIRD, MINOR_THIRD, MAJOR_SECOND};
const vector<int> MAJOR_SEVENTH_CHORD_INTERVALS = {MAJOR_THIRD, MINOR_THIRD, MAJOR_THIRD, MINOR_SECOND};
const vector<int> MINOR_SEVENTH_CHORD_INTERVALS = {MINOR_THIRD, MAJOR_THIRD, MINOR_THIRD, MAJOR_SECOND};

const vector<vector<int>> chordQualitiesIntervals = {MAJOR_CHORD_INTERVALS, MINOR_CHORD_INTERVALS,
                                                     DIMINISHED_CHORD_INTERVALS,AUGMENTED_CHORD_INTERVALS,
                                                     DOMINANT_SEVENTH_CHORD_INTERVALS, MAJOR_SEVENTH_CHORD_INTERVALS,
                                                     MINOR_SEVENTH_CHORD_INTERVALS};

const vector<std::string> chordQualityNames = {"Major", "Minor", "Diminished", "Augmented", "Dominant seventh",
                                               "Major seventh", "Minor seventh"};

// Chord states
enum chordStates{
    FUNDAMENTAL_STATE,  //0
    FIRST_INVERSION,    //1
    SECOND_INVERSION,   //2
    THIRD_INVERSION     //3
};

const vector<std::string> stateNames = {"Fundamental state", "First inversion", "Second inversion", "Third inversion"};

/** Modes */
enum Mode {
    IONIAN,     //0 , major mode
    DORIAN,     //1
    PHRYGIAN,   //2
    LYDIAN,     //3
    MIXOLYDIAN, //4
    AEOLIAN,    //5 , natural minor mode
    LOCRIAN     //6
};

// syntactic sugar for more commonly used modes
const int MAJOR_MODE = IONIAN;
const int MINOR_MODE = AEOLIAN;

const vector<std::string> modeNames = {"Major", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Minor", "Locrian"};

/** Scales */
/// defined by the intervals between their notes

//@todo turn this into a dictionary with the name of the scale as key and the vector of intervals as value

const vector<int> MAJOR_SCALE = {MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND};
const vector<int> NATURAL_MINOR_SCALE = {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND};
const vector<int> HARMONIC_MINOR_SCALE = {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND, AUGMENTED_SECOND, MINOR_SECOND};
const vector<int> MELODIC_MINOR_SCALE = {MAJOR_SECOND, MINOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MAJOR_SECOND, MINOR_SECOND};

/***********************************************************************************************************************
 *                                                                                                                     *
 *                                                      Functions                                                      *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/**
 * For a given set of intervals between notes that loops and a starting note, returns all the possible notes
 * @param note the starting note
 * @param intervals the set of intervals between notes. It must make a loop. For example, to get all notes from a major
 * scale from note, use {2, 2, 1, 2, 2, 2, 1}. To get all notes from a minor chord, use {3, 4, 5}.
 * @return vector<int> all the notes
 */
vector<int> get_all_notes_from_interval_loop(int n, vector<int> intervals);

/**
 * For a given tonality (root + mode), returns all the possible notes
 * @param root the root of the tonality (in [0,11])
 * @param scale the set of tones and semitones that define the scale
 * @return vector<int> all the possible notes from that tonality
 */
vector<int> get_all_notes_from_scale(int root, vector<int> scale);

/**
 * For a given chord (root + mode), returns all the possible notes
 * @param root the root of the chord
 * @param quality the set of tones and semitones that define the chord
 * @return vector<int> all the possible notes from that chord
 */
vector<int> get_all_notes_from_chord(int root, vector<int> quality);

/**
 * Get all values for a given note
 * @param note a note
 * @return vector<int> a vector containing all the given notes
 */
vector<int> get_all_given_note(int note);

/**
 * Transforms an int* into a vector<int>
 * @param ptr an int* pointer
 * @param size the size of the array
 * @return a vector<int> containing the same values as the array
 */
vector<int> int_pointer_to_vector(int* ptr, int size);

/**
 * Transforms a vector of integers into a string
 * @param vector a vector of integers
 * @return string the string representation of the vector
 */
string int_vector_to_string(vector<int> vector);

/**
 * Prints the Search::Statistics object into a readable format
 * @param stats a Search::Statistics object representing the statistics of a search
 * @return The string representation of the statistics object
 */
string statistics_to_string(Search::Statistics stats);

/**
 * Prints the Search::Statistics object into a csv format (coma separated)
 * @param stats a Search::Statistics object representing the statistics of a search
 * @return The string representation of the statistics object
 */
string statistics_to_csv_string(Search::Statistics stats);

/**
 * Returns the value of a variable as a string
 * @param var an integer variable
 * @param absolute a boolean indicating if the value should be returned as an absolute value (default is false)
 * @return a string representing the value of the variable
 */
string intVar_to_string(const IntVar &var, bool absolute = false);

/**
 * Returns the values of an array of variables as a string. Calls the intVar_to_string function
 * @param vars an array of integer variables
 * @return a string representing the values of the variables
 */
string intVarArray_to_string(IntVarArray vars);

/**
 * Returns the values of an IntVarArgs as a string
 * @param args an IntVarArgs
 * @return a string representing the values
 */
string intVarArgs_to_string(IntVarArgs args);

/**
 * Returns the name of a note based on its MIDI value
 * @param note an integer
 */
string midi_to_letter(int note);

/**
 * Returns the name of a mode based on its integer value
 * @param mode an integer
 * @return a string representing the name of the mode
 */
string mode_int_to_name(int mode);

/**
 * returns a string with the time
 * @return a string with the time
 */
string time();

/**
 * Write a text into a log file
 * @param message the text to write
 */
void write_to_log_file(const char *message, const string& filename);

#endif