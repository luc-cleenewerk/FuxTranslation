#include "headers/Utilities.hpp"

/**
 * For a given set of intervals between notes that loops and a starting note, returns all the possible notes
 * @param note the starting note
 * @param intervals the set of intervals between notes. It must make a loop. For example, to get all notes from a major
 * scale from note, use {2, 2, 1, 2, 2, 2, 1}. To get all notes from a minor chord, use {3, 4, 5}.
 * @return vector<int> all the notes
 */
vector<int> get_all_notes_from_interval_loop(int n, vector<int> intervals)
{
    int note = n % PERFECT_OCTAVE; // bring the root back to [12,23] in case the argument is wrong
    vector<int> notes;

    int i = 0;
    while (note <= 127)
    {
        notes.push_back(note);
        note += intervals[i % intervals.size()];
        ++i;
    }
    return notes;
}

/**
 * For a given tonality (root + mode), returns all the possible notes
 * @param root the root of the tonality (in [0,11])
 * @param scale the set of tones and semitones that define the scale
 * @return vector<int> all the possible notes from that tonality
 */
vector<int> get_all_notes_from_scale(int root, vector<int> scale)
{
    return get_all_notes_from_interval_loop(root, scale);
}

/**
 * For a given chord (root + mode), returns all the possible notes
 * @param root the root of the chord
 * @param quality the set of tones and semitones that define the chord
 * @return vector<int> all the possible notes from that chord
 */
vector<int> get_all_notes_from_chord(int root, vector<int> quality)
{
    return get_all_notes_from_interval_loop(root, quality);
}

vector<int> intersection(vector<int> v1, vector<int> v2){
    vector<int> result;
    sort(v1.begin(), v1.end());
    sort(v2.begin(), v2.end());

    set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), back_inserter(result));

    return result;
}

/**
 * Get all values for a given note
 * @param note a note
 * @return vector<int> a vector containing all the given notes
 */
vector<int> get_all_given_note(int note)
{
    int current = note % PERFECT_OCTAVE;
    vector<int> notes;
    while (current < 127)
    {
        notes.push_back(current);
        current += 12;
    }
    return notes;
}

/**
 * Transforms a vector of integers into a string
 * @param vector a vector of integers
 * @return string the string representation of the vector
 */
string int_vector_to_string(vector<int> vector){
    string s;
    for (int i = 0; i < vector.size(); ++i) {
        s += to_string(vector[i]);
        if(i != vector.size() - 1)
            s += " , ";
    }
    return s;
}

/**
 * Transforms an int* into a vector<int>
 * @param ptr an int* pointer
 * @param size the size of the array
 * @return a vector<int> containing the same values as the array
 */
vector<int> int_pointer_to_vector(int* ptr, int size){
    vector<int> v;
    for(int i = 0; i < size; i++){
        v.push_back(ptr[i]);
    }
    return v;
}

/**
 * Prints the Search::Statistics object into a readable format
 * @param stats a Search::Statistics object representing the statistics of a search
 * @return The string representation of the statistics object
 */
string statistics_to_string(Search::Statistics stats){
    string s = "Nodes traversed: " + to_string(stats.node) + "\n";
    s += "Failed nodes explored: " + to_string(stats.fail) + "\n";
    s += "Restarts performed: " + to_string(stats.restart) + "\n";
    s += "Propagators executed: " + to_string(stats.propagate) + "\n";
    s += "No goods generated: " + to_string(stats.nogood) + "\n";
    s += "Maximal depth of explored tree: " + to_string(stats.depth) + "\n";
    return s;
}

/**
 * Prints the Search::Statistics object into a csv format (coma separated)
 * @param stats a Search::Statistics object representing the statistics of a search
 * @return The string representation of the statistics object
 */
string statistics_to_csv_string(Search::Statistics stats){
    string s = to_string(stats.node) + ",";
    s += to_string(stats.fail) + ",";
    s += to_string(stats.restart) + ",";
    s += to_string(stats.propagate) + ",";
    s += to_string(stats.nogood) + ",";
    s += to_string(stats.depth) + ",";
    return s;
}

/**
 * Returns the value of a variable as a string
 * @param var an integer variable
 * @param absolute a boolean indicating if the value should be returned as an absolute value (default is false)
 * @return a string representing the value of the variable
 */
string intVar_to_string(const IntVar &var, bool absolute) {
    if (var.assigned()){
        if(absolute)
            return to_string(abs(var.val()));
        return to_string(var.val());
    }
    return "<not assigned>";
}

/**
 * Returns the values of an array of variables as a string
 * @param vars an array of integer variables
 * @return a string representing the values of the variables
 */
string intVarArray_to_string(IntVarArray vars){
    int s = vars.size();
    string res = "{";
    for(int i = 0; i < s; i++){
        res += intVar_to_string(vars[i]);
        if(i != s - 1)
            res += ", ";
    }
    res += "}";
    return res;
}

/**
 * Returns the values of an IntVarArgs as a string
 * @param args an IntVarArgs
 * @return a string representing the values
 */
string intVarArgs_to_string(IntVarArgs args){
    int s = args.size();
    string res;
    for(int i = 0; i < s; i++){
        res += intVar_to_string(args[i], 0);
        if(i != s - 1)
            res += ", ";
    }
    return res;
}



/**
 * Returns the name of a note based on its MIDI value
 * @param note an integer
 */
string midi_to_letter(int note){
    return noteNames[note % PERFECT_OCTAVE];
}

/**
 * Returns the name of a mode based on its integer value
 * @param mode an integer
 * @return a string representing the name of the mode
 */
string mode_int_to_name(int mode){
    return modeNames[mode];
}

/**
 * returns a string with the time
 * @return a string with the time
 */
string time(){
    /// date and time for logs
    std::time_t currentTime = std::time(nullptr); // Get the current time
    std::string timeString = std::asctime(std::localtime(&currentTime)); // Convert to string
    return timeString;
}

/**
 * Write a text into a log file
 * Useful for debugging in the OM environment
 * @param message the text to write
 */
void write_to_log_file(const char *message, const string& filename) {
    const char* homeDir = std::getenv("HOME"); // Get the user's home directory
    if (homeDir) {
        std::string filePath(homeDir);
        filePath += "/Documents/Libraries/MusicConstraints/out/" + filename; // Specify the desired file path, such as $HOME/log.txt

        std::ofstream myfile(filePath, std::ios::app); // append mode
        if (myfile.is_open()) {
            myfile << message << endl;
            myfile.close();
        }
    }
}

void create_solution_array(IntVarArray sol, vector<Part> parts){
    int idx = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < parts[p].sol_len; i++){
            sol[idx] = parts[p].solution_array[i];
            idx++;
        }
    }
}

void add_fifth_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs y(splist.size()*size);
    int index = 0;

    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            y[index] = parts[p].fifth_costs[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(y)));
}

void add_octave_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs oc(splist.size()*size);
    int index = 0;

    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            oc[index] = parts[p].octave_costs[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(oc)));
}

void add_off_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs z(splist.size()*size);
    int index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size; i++){
            z[index] = parts[p].off_costs[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(z)));
}

void add_melodic_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs x((splist.size()*size)-splist.size());
    int index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            x[index] = parts[p].m_degrees_cost[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(x)));
}

void add_motion_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs pc((splist.size()*size)-splist.size());
    int index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            pc[index] = parts[p].motions_cost[0][i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(pc)));
}

void add_variety_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs v((splist.size()*size)-splist.size());
    int index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-1; i++){
            v[index] = parts[p].varietyArray[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(v)));
}

void add_succ_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Part> parts){
    IntVarArgs scc((splist.size()*size)-2*splist.size());
    int index = 0;
    for(int p = 1; p < parts.size(); p++){
        for(int i = 0; i < size-2; i++){
            scc[index] = parts[p].succ_cost[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(scc)));
}

void add_triad_cost(const Home &home, IntVar cost_factor, int size, vector<int> splist, vector<Stratum> upper){
    IntVarArgs tr(upper.size()*size);
    int index = 0;
    for(int p = 0; p < upper.size(); p++){
        for(int i = 0; i < size; i++){
            tr[index] = upper[p].triad_costs[i];
            index++;
        }
    }
    rel(home, cost_factor, IRT_EQ, expr(home, sum(tr)));
}