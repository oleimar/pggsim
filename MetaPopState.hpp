#ifndef METAPOPSTATE_HPP
#define METAPOPSTATE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// The EvoProg program runs actor-critic learning simulations
// Copyright (C) 2019  Olof Leimar
// See Readme.md for copyright notice

// The purpose of this code is to implement 'population containers' for
// individual-based simulations, in the form of containers of subpopulations.
// In addition to the individuals in a subpopulation, each subpopulation has a
// state, which can simply be a subpopulation number, but can also express
// other properties, like environmental conditions for a subpopulation.

// a default subpopulation state, giving only the subpopulation number
struct PopNum {
public:
    std::size_t spn;
};

//*************************** SubPop0 *******************************

// This is a container for a subpopulation. The methods Add() and Remove()
// ensure that all individuals that are 'present' in the subpopulation are
// 'alive'. However, it differs from the other subpopulation containers below
// in that it assumed that individuals are not set dead (there is no Remove())
// method, but are simply added to start a generation. It does, however, have a
// method clear() that removes all individuals

// NOTE: The struct has a non-const operator[](std::size_t) but access through
// this operator should not be used to set individuals alive or dead, since
// that should be handled by Add() and clear(). The purpose of the operator is
// instead to allow other kinds of changes of an individual's state.

// It is possible to iterate over the members of a SubPop1 sp as follows:
// for (int i = 0; i < sp.Iend(); ++i) ...
// or
// for (int i = 0; i < sp.size(); ++i) ...
// because there are no "dead" individuals in between

// Assumptions about template parameter struct Individual:
// 1. It has a default constructor (should construct "dead" individual)
// 2. Member functions:
//       bool Alive()
//       void SetAlive()
//       void SetDead()

// The template parameter struct State is intended to hold the subpopulation
// number and local environmental states of the subpopulation, but it is not
// made use of here.

template <typename Individual, typename State = PopNum>
struct SubPop0
{
// public:
    using ind_type = Individual;
    using state_type = State;
    SubPop0(std::size_t a_max_inds = 0) :
    max_inds{a_max_inds} { ind.reserve(max_inds); }
    void Assign(std::size_t a_max_inds = 0);
    void clear();
    Individual& operator[](std::size_t i ) { return ind[i]; }
    const Individual& operator[](std::size_t i ) const { return ind[i]; }
    std::size_t size() const { return ind.size(); }
    std::size_t Iend() const { return ind.size(); }
    std::size_t NumInds() const { return ind.size(); }
    std::size_t MaxInds() const { return max_inds; }
    bool Full() const { return ind.size() == max_inds; }
    void Add(Individual&& indi);
    void Add(const Individual& indi);
    // void Remove(std::size_t i);
    State& GetState() { return st; }
    const State& GetState() const { return st; }
    // Swap the individuals (but not the state) of subpops
    void swap(SubPop0<Individual, State>& other_pop);
    // public data members
    std::vector<Individual> ind;
    std::size_t max_inds;
    State st;
};

template <typename Individual, typename State>
void SubPop0<Individual,State>::Assign(std::size_t a_max_inds)
{
    ind.clear();
    max_inds = a_max_inds;
    ind.reserve(max_inds);
}

template <typename Individual, typename State>
void SubPop0<Individual,State>::clear()
{
    // for (std::size_t i = 0; i < ind.size(); ++i) ind[i].SetDead();
    ind.clear();
}

template <typename Individual, typename State>
void SubPop0<Individual,State>::Add(Individual&& indi)
{
    if (ind.size() < max_inds) {
        ind.emplace_back(indi);
    }
}

template <typename Individual, typename State>
void SubPop0<Individual,State>::Add(const Individual& indi)
{
    if (ind.size() < max_inds) {
        ind.push_back(indi);
    }
}

template <typename Individual, typename State>
void SubPop0<Individual, State>::swap(SubPop0<Individual, State>& other_pop)
{
    // Swap the individuals (but not the state) of subpops
    // swap subpops and free indicator
    ind.swap(other_pop.ind);
    // swap max_inds
    std::size_t save = max_inds;
    max_inds = other_pop.max_inds;
    other_pop.max_inds = save;
}


//*************************** SubPop1 *******************************

// This is a container for a subpopulation. The methods Add() and Remove()
// ensure that all individuals that are 'present' in the subpopulation are
// 'alive'.

// NOTE: The class has a non-const operator[](std::size_t) but access through
// this operator should not be used to set individuals alive or dead, since
// that should be handled by Add() and Remove(). The purpose of the operator is
// instead to allow other kinds of changes of an individual's state.

// It is possible to iterate over the members of a SubPop1 sp as follows:
// for (int i = 0; i < sp.Iend(); ++i) if (sp[i].Alive()) ...

// Assumptions about template parameter class Individual:
// 1. It has a default constructor (should construct "dead" individual)
// 2. Member functions:
//       bool Alive()
//       void SetAlive()
//       void SetDead()

// The template parameter class State is intended to hold the subpopulation
// number and local environmental states of the subpopulation, but it is not
// made use of here.

template <typename Individual, typename State = PopNum>
class SubPop1
{
public:
    using ind_type = Individual;
    using state_type = State;
    SubPop1(std::size_t max_inds = 0) :
        ind(max_inds),
        free(max_inds),
        num_free{max_inds},
        i_end{0}
    { for (std::size_t k = 0; k < max_inds; ++k) free[k] = max_inds - 1 - k; }
    void Assign(std::size_t max_inds = 0);
    void clear();
    Individual& operator[](std::size_t i ) { return ind[i]; }
    const Individual& operator[](std::size_t i ) const { return ind[i]; }
    std::size_t NumInds() const { return ind.size() - num_free; }
    std::size_t MaxInds() const { return ind.size(); }
    bool Full() const { return (num_free == 0); }
    std::size_t Iend() const { return i_end; }
    void Add(const Individual& indi);
    void Remove(std::size_t i);
    State& GetState() { return st; }
    // Swap the individuals (but not the state) of subpops
    void swap(SubPop1<Individual, State>& other_pop);

private:
    std::vector<Individual> ind;
    std::vector<std::size_t> free;
    std::size_t num_free;
    std::size_t i_end;
    State st;
};

template <typename Individual, typename State>
void SubPop1<Individual,State>::Assign(std::size_t max_inds)
{
    ind.assign(max_inds, Individual());
    free.assign(max_inds, 0);
    num_free = max_inds;
    i_end = 0;
    for (std::size_t k = 0; k < max_inds; ++k) free[k] = max_inds - 1 - k;
}

template <typename Individual, typename State>
void SubPop1<Individual,State>::clear()
{
    for (std::size_t i = 0; i < ind.size(); ++i) ind[i].SetDead();
    for (std::size_t k = 0; k < ind.size(); ++k) free[k] = ind.size() - 1 - k;
    num_free = ind.size();
    i_end = 0;
}

template <typename Individual, typename State>
void SubPop1<Individual,State>::Add(const Individual& indi)
{
    if (num_free > 0) {
        --num_free;
        std::size_t i = free[num_free];
        ind[i] = indi;
        ind[i].SetAlive();
        if (i >= i_end) i_end = i + 1;
    }
}

template <typename Individual, typename State>
void SubPop1<Individual,State>::Remove(std::size_t i)
{
    if (ind[i].Alive()) {
        free[num_free] = i;
        ++num_free;
        ind[i].SetDead();
        if (i == i_end - 1) {
            --i_end;
            while (i_end > 0 && !ind[i_end - 1].Alive()) --i_end;
        }
    }
}

template <typename Individual, typename State>
void SubPop1<Individual,State>::swap(SubPop1<Individual,State>& other_pop)
{
    // Swap the individuals (but not the state) of subpops
    // swap subpops and free indicator
    ind.swap(other_pop.ind);
    free.swap(other_pop.free);
    // swap num_free and i_end
    std::size_t save = num_free;
    num_free = other_pop.num_free;
    other_pop.num_free = save;
    save = i_end;
    i_end = other_pop.i_end;
    other_pop.i_end = save;
}


//************************** SubPopStruct1 **************************

// The purpose of this class is to construct an array of the indices of the
// (living) members in a SubPop object

// Assumptions about template parameter class SubPop:
// 1. Member functions:
//       std::size_t NumInds()
//       std::size_t Iend()
//       const Individual& operator[](std::size_t)
//       where the type Individual has a member function
//       bool Alive()

template <typename SubPop>
class SubPopStruct1
{
public:
    SubPopStruct1(const SubPop& sub_pop);
    void Assign(const SubPop& sub_pop);
    std::size_t N() const { return n; }
    std::size_t Index(std::size_t k) const { return alive_sub_pop[k]; }
    void UpdateDeath(std::size_t k);
private:
    std::vector<std::size_t> alive_sub_pop;
    std::size_t n;
};

template <typename SubPop>
SubPopStruct1<SubPop>::
SubPopStruct1(const SubPop& sub_pop) :
    alive_sub_pop(sub_pop.NumInds()),
    n(sub_pop.NumInds())
{
    std::size_t k = 0;
    for (std::size_t i = 0; i < sub_pop.Iend(); ++i) {
        if (sub_pop[i].Alive()) alive_sub_pop[k++] = i;
    }
}

template <typename SubPop>
void SubPopStruct1<SubPop>::
Assign(const SubPop& sub_pop)
{
    alive_sub_pop.assign(sub_pop.NumInds(), 0);
    n = sub_pop.NumInds();
    std::size_t k = 0;
    for (std::size_t i = 0; i < sub_pop.Iend(); ++i) {
        if (sub_pop[i].Alive()) alive_sub_pop[k++] = i;
    }
}

template <typename SubPop>
void SubPopStruct1<SubPop>::
UpdateDeath(std::size_t k)
{
    // make adjustments for the death of the individual with
    // its index in position k of alive_sub_pop
    if (k == n - 1) {
        // no adjustment of alive_sub_pop is needed
    } else {
        // Replace dead individual with last one
        alive_sub_pop[k] = alive_sub_pop[n - 1];
    }
    -- n; // One less alive
}


//*************************** SubPop2 *******************************

// This is a container for a subpopulation of individuals with two sexes. The
// methods Add() and Remove() ensure that all individuals that are 'present' in
// the subpopulation are 'alive'.

// NOTE: The class has a non-const operator[](std::size_t) but access through
// this operator should not be used to set individuals alive or dead, since
// that should be handled by Add() and Remove(). Also, the sex of individuals
// should not be changed while they are in the subpopulation. The purpose of
// the operator[](std::size_t) is instead to allow other kinds of changes of an
// individual's state.

// It is possible to iterate over the members of a SubPop2 sp as follows:
// for (int i = 0; i < sp.Iend(); ++i) if (sp[i].Alive()) ...

// Assumptions about template parameter class Individual:
// 1. It has a default constructor (should construct "dead" individual)
// 2. Member functions:
//       bool Alive()
//       void SetAlive()
//       void SetDead()
//       bool Female()

// The template parameter class State is intended to hold the subpopulation
// number and local environmental states of the subpopulation, but it is not
// made use of here.

template <typename Individual, typename State = PopNum>
class SubPop2
{
public:
    using ind_type = Individual;
    using state_type = State;
    SubPop2(std::size_t max_inds = 0) :
        ind(max_inds),
        free(max_inds),
        num_free{max_inds},
        i_end{0},
        nf{0},
        nm{0}
    { for (std::size_t k = 0; k < max_inds; ++k) free[k] = max_inds - 1 - k; }
    void Assign(std::size_t max_inds = 0);
    void clear();
    Individual& operator[](std::size_t i ) { return ind[i]; }
    const Individual& operator[](std::size_t i ) const { return ind[i]; }
    std::size_t NumInds() const { return ind.size() - num_free; }
    std::size_t MaxInds() const { return ind.size(); }
    bool Full() const { return (num_free == 0); }
    std::size_t Iend() const { return i_end; }
    std::size_t Nf() const { return nf; }
    std::size_t Nm() const { return nm; }
    void Add(const Individual& indi);
    void Remove(std::size_t i);
    State& GetState() { return st; }
    // Swap the individuals (but not the state) of subpops
    void swap(SubPop2<Individual,State>& other_pop);

private:
    std::vector<Individual> ind;
    std::vector<std::size_t> free;
    std::size_t num_free;
    std::size_t i_end;
    std::size_t nf;
    std::size_t nm;
    State st;
};

template <typename Individual, typename State>
void SubPop2<Individual, State>::Assign(std::size_t max_inds)
{
    ind.assign(max_inds, Individual());
    free.assign(max_inds, 0);
    num_free = max_inds;
    i_end = 0;
    nf = 0;
    nm = 0;
    for (std::size_t k = 0; k < max_inds; ++k) free[k] = max_inds - 1 - k;
}

template <typename Individual, typename State>
void SubPop2<Individual, State>::clear()
{
    for (std::size_t i = 0; i < ind.size(); ++i) ind[i].SetDead();
    for (std::size_t k = 0; k < ind.size(); ++k) free[k] = ind.size() - 1 - k;
    num_free = ind.size();
    i_end = 0;
    nf = 0;
    nm = 0;
}

template <typename Individual, typename State>
void SubPop2<Individual, State>::Add(const Individual& indi)
{
    if (num_free > 0) {
        --num_free;
        std::size_t i = free[num_free];
        ind[i] = indi;
        ind[i].SetAlive();
        if (i >= i_end) i_end = i + 1;
        if (ind[i].Female()) ++nf;
        else ++nm;
    }
}

template <typename Individual, typename State>
void SubPop2<Individual, State>::Remove(std::size_t i)
{
   if (ind[i].Alive()) {
        free[num_free] = i;
        if ( ind[i].Female() ) --nf;
        else --nm;
        ++num_free;
        ind[i].SetDead();
        if ( i == i_end - 1 ) {
            --i_end;
            while (i_end > 0 && !ind[i_end - 1].Alive()) --i_end;
        }
    }
}

template <typename Individual, typename State>
void SubPop2<Individual, State>::swap(SubPop2<Individual,State>& other_pop)
{
    // Swap the individuals (but not the state) of subpops
    // swap subpops and free indicator
    ind.swap(other_pop.ind);
    free.swap(other_pop.free);
    // swap num_free and i_end
    std::size_t save = num_free;
    num_free = other_pop.num_free;
    other_pop.num_free = save;
    save = i_end;
    i_end = other_pop.i_end;
    other_pop.i_end = save;
    save = nf;
    nf = other_pop.nf;
    other_pop.nf = save;
    save = nm;
    nm = other_pop.nm;
    other_pop.nm = save;
}


//************************** SubPopStruct2 **************************

// The purpose of this class is to construct an array of the indices of the
// (living) members in a SubPop object

// Assumptions about template parameter class SubPop:
// 1. Member functions:
//       std::size_t NumInds()
//       std::size_t Nf()
//       std::size_t Nm()
//       std::size_t Iend()
//       const Individual& operator[](std::size_t)
//       where the type Individual has a member function
//       bool Alive()
//       bool Female()

template <typename SubPop>
class SubPopStruct2
{
public:
    SubPopStruct2(const SubPop& sub_pop);
    void Assign(const SubPop& sub_pop);
    std::size_t N() const { return n; }
    std::size_t Nf() const { return nf; }
    std::size_t Nm() const { return nm; }
    std::size_t IndexF(std::size_t k) const { return female[k]; }
    std::size_t IndexM(std::size_t k) const { return male[k]; }
    void UpdateDeathF(std::size_t k);
    void UpdateDeathM(std::size_t k);
private:
    std::vector<std::size_t> female;
    std::vector<std::size_t> male;
    std::size_t n;
    std::size_t nf;
    std::size_t nm;
};

template <typename SubPop>
SubPopStruct2<SubPop>::
SubPopStruct2(const SubPop& sub_pop) :
    female(sub_pop.Nf()),
    male(sub_pop.Nm()),
    n(sub_pop.NumInds()),
    nf(sub_pop.Nf()),
    nm(sub_pop.Nm())
{
    std::size_t kf = 0;
    std::size_t km = 0;
    for (std::size_t i = 0; i < sub_pop.Iend(); ++i) {
        if ( sub_pop[i].Alive() ) {
            if ( sub_pop[i].Female() ) female[kf++] = i;
            else male[km++] = i;
        }
    }
}

template <typename SubPop>
void SubPopStruct2<SubPop>::
Assign(const SubPop& sub_pop)
{
    female.assign(sub_pop.Nf(), 0);
    male.assign(sub_pop.Nm(), 0);
    n = sub_pop.NumInds();
    nf = sub_pop.Nf();
    nm = sub_pop.Nm();
    std::size_t kf = 0;
    std::size_t km = 0;
    for (std::size_t i = 0; i < sub_pop.Iend(); ++i) {
        if (sub_pop[i].Alive()) {
            if (sub_pop[i].Female()) female[kf++] = i;
            else male[km++] = i;
        }
    }
}

template <typename SubPop>
void SubPopStruct2<SubPop>::
UpdateDeathF(std::size_t k)
{
    // make adjustments for the death of the female with
    // index in position k of female
    if (k == nf - 1) {
        // no adjustment of female is needed
    } else {
        female[k] = female[nf - 1]; // Replace dead female with last one
    }
    --nf; // One less alive
}

template <typename SubPop>
void SubPopStruct2<SubPop>::
UpdateDeathM(std::size_t k)
{
    // make adjustments for the death of the male with
    // index in position k of male
    if ( k == nm - 1 ) {
        // no adjustment of male is needed
    } else {
        male[k] = male[nm-1]; // Replace dead male with last one
    }
    --nm; // One less alive
}


//*************************** MetaPopState ********************************

// The purpose of this class is to be a container of subpopulations with a
// state that includes an integer-valued index (subpopulation number). The
// class does however not do anything with the states or subpopulation numbers,
// except that it checks for valid number when reading a MetaPopState from file
// (and inserts the individual read into the corresponding subpopulation). The
// states of subpopulations are not read from file (it is assumed that
// individuals can contain information about the state of the subpopulation
// they belong to).

// Assumptions about template parameter class SubPop:
// 1. It has a constructor SupPop(std::size_t max_inds)
// 2. It has a public type ind_type
// 3. Member functions:
//       void Add(const ind_type& indi)
//       bool Full()
//       std::size_t Iend()
//       const ind_type& operator[](std::size_t i)

// Assumptions about SubPop::ind_type:
// 1. It has a default constructor (should construct "dead" individual)
// 2. Member functions:
//       bool Alive()
//       std::size_t SubPopNum()
// 3. Static member function
//       std::string ColHeads()
// 4. Input and output operators >> and <<

template <typename SubPop>
class MetaPopState
{
public:
    using ind_type = typename SubPop::ind_type;
    MetaPopState(std::size_t num_p = 0, std::size_t max_inds = 0) :
        sub_pop(num_p, SubPop(max_inds)) {}
    void Assign(std::size_t num_p, std::size_t max_inds);
    SubPop& operator[](std::size_t k) { return sub_pop[k]; }
    const SubPop& operator[](std::size_t k) const { return sub_pop[k]; }
    std::size_t NumPops() const { return sub_pop.size(); }
    void swap(MetaPopState<SubPop>& other_pop);
    // Read_from_File checks that subpopulation numbers are valid
    bool Read_from_File(const std::string& infilename, std::size_t n);
    void Write_to_File(const std::string& outfilename) const;
private:
    std::vector<SubPop> sub_pop;
};

template <typename SubPop>
void MetaPopState<SubPop>::Assign(std::size_t num_p, std::size_t max_inds)
{
    sub_pop.assign(num_p, SubPop(max_inds));
}

template <typename SubPop>
void MetaPopState<SubPop>::swap(MetaPopState<SubPop>& other_pop)
{
    sub_pop.swap(other_pop.sub_pop);
}

template <typename SubPop>
bool MetaPopState<SubPop>::Read_from_File(const std::string& infilename,
                                          std::size_t n)
{
    bool OK = true;
    std::size_t n_inds = 0;
    std::ifstream infile(infilename.c_str());
    if (!infile) {
        std::cerr << "Could not open file " << infilename << '\n';
        OK = false;
    } else {
        // skip first line in file (contains headers)
        char c = '\0';
        while (c != '\n' && infile) infile.get(c);
        if (infile) {
            ind_type indi;
            while (infile >> indi) {
                std::size_t spn = indi.SubPopNum();
                // check if subpopulation number is valid
                if (spn < sub_pop.size()) {
                    if (!sub_pop[spn].Full()) {
                        if (indi.Alive()) {
                            sub_pop[spn].Add(indi);
                            ++n_inds; // count individuals read
                        }
                    } else {
                        std::cerr << "Subpopulation number "
                                  << spn
                                  << " is full\n";
                        OK = false;
                    }
                } else {
                    std::cerr << "Individual\n"
                              << indi
                              << "\nhas invalid subpopulation number: "
                              << spn << '\n';
                    OK = false;
                }
            }
        } else {
            std::cerr << "No data to read from " << infilename << "!\n";
            OK = false;
        }
        infile.close();
    }
    if (n_inds != n) {
        OK = false;
    }
    return OK;
}

template <typename SubPop>
void MetaPopState<SubPop>::Write_to_File(const std::string& outfilename) const
{
    std::ofstream outfile(outfilename.c_str(), std::ios_base::out);
    if (!outfile) {
        std::cout << "Cannot open " << outfilename << ", cannot save data \n";
    } else {
        outfile << ind_type::ColHeads() << '\n';
        for (std::size_t k = 0; k < sub_pop.size(); ++k) {
            for (std::size_t i = 0; i < sub_pop[k].Iend(); ++i) {
                if (sub_pop[k][i].Alive()) outfile << sub_pop[k][i] << '\n';
            }
        }
        outfile.close();
    }
}


#endif // METAPOPSTATE_HPP
