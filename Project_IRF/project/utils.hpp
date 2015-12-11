//
//  utils.hpp
//  ProjectIRF
//
//  Created by Cyrille Delabre on 11/12/2015.
//  Copyright © 2015 Cyrille Delabre. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp



class utils
{
public:
    
    //debugging on console
    bool VERBOSE =false;
    bool RESULT = false;;
    
    
    //remove all sources files before commit
    bool REMOVE_SOURCE = false;
    
    //use folder test or default folder
    bool TEST = false;
    
    
    bool EXTRACT_IMAGES = false;
    bool GET_FEATURES = false;
    bool NORMALIZE = false;
    bool CREATE_VARIOUS = true;
    
    int SPLIT_FACTOR = -1;

    
    static utils& i()
    {
        static utils instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
private:
    utils() {};                   // Constructor? (the {} brackets) are needed here.
    
    // C++ 03
    // ========
    // Dont forget to declare these two. You want to make sure they
    // are unacceptable otherwise you may accidentally get copies of
    // your singleton appearing.
    utils(utils const&);              // Don't Implement
    void operator=(utils const&); // Don't implement
};



static inline string niceOutput(string output, bool ok){
    if(false){
        stringstream ss;
        int  FG_RED      = 31;
        int FG_GREEN    = 32;
        ss << "\033[";
        if(ok){
            ss << FG_GREEN << "m" << output;
        }else{
            ss << FG_RED << "m"  << output;
        }
        
        ss << " \033[0m";
        return ss.str();
    }
    return output;
}


#endif /* utils_hpp */
