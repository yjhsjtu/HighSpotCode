// HighSpotCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "json.hpp"
#include <iomanip>
#include <unordered_map>
#include <unordered_set>

using namespace std;
using namespace nlohmann::detail;
using json = nlohmann::json;

struct User
{
    string id;
    string name;
};

void to_json(json& j, const User& user) {
    j = json{ {"id", user.id}, {"name", user.name} };
}

void from_json(const json& j, User& user) {
    j.at("id").get_to(user.id);
    j.at("name").get_to(user.name);
}

struct Playlist
{
    string id;
    string user_id;
    unordered_set<string> song_ids;
};

void to_json(json& j, const Playlist& playlist) {
    j = json{ {"id", playlist.id}, {"song_ids", playlist.song_ids},{"user_id", playlist.user_id}};
}

void from_json(const json& j, Playlist& playlist) {
    j.at("id").get_to(playlist.id);
    j.at("user_id").get_to(playlist.user_id);
    j.at("song_ids").get_to(playlist.song_ids);
}

struct Song
{
    string id;
    string artist;
    string title;
};

void to_json(json& j, const Song& song) {
    j = json{ {"id", song.id}, {"artist", song.artist}, {"title", song.title} };
}

void from_json(const json& j, Song& song) {
    j.at("id").get_to(song.id);
    j.at("artist").get_to(song.artist);
    j.at("title").get_to(song.title);
}

struct UpdatedPlaylist
{
    string id;
    vector<string> song_ids;
};

void from_json(const json& j, UpdatedPlaylist& updatedplaylist) {
    j.at("id").get_to(updatedplaylist.id);
    j.at("song_ids").get_to(updatedplaylist.song_ids);
}

struct Removedplaylist
{
    string id;
};

void from_json(const json& j, Removedplaylist& removedplaylist) {
    j.at("id").get_to(removedplaylist.id);
}

// Method to show the command help information
static void show_usage(string name)
{
    std::cerr << "Usage: "<<name<<" <input.json> <change.json> <output.json>\n" 
        << "\t-h,--help\t\tShow this help message\n"
        << std::endl;
}

// Validate the data of the input file.
// It check 1) if refered data is meaningful. For example, user id and song id in playlist exist or not
// 2) if duplicated data like user or song exist
int ValidateInputFile(json& input,
    unordered_map<string, User>& users,
    unordered_map<string, Song>& songs,
    unordered_map<string,Playlist>& playlists)
{
    // parse users elements
    auto const it_users = input.find("users");
    if (it_users == input.end())
    {
        std::cerr << "Users can not be found";
        return 1;
    }

    for (auto it : *it_users)
    {
        User user;
        from_json(it, user);
        if (users.count(user.id))
        {
            std::cerr << "Duplicated user element found in input file " << user.id << endl;
            return 1;
        }
        users[user.id]=user;
    }

    // parse songs elements
    auto const it_songs = input.find("songs");
    if (it_songs == input.end())
    {
        std::cerr << "Songs can not be found";
        return 1;
    }

    for (auto it : *it_songs)
    {
        Song song;
        from_json(it, song);
        if (songs.count(song.id))
        {
            std::cerr << "Duplicated song element found in input file" << song.id << endl;
            return 1;
        }

        songs[song.id]=song;
    }

    // parse playlists elements
    auto const it_playlists = input.find("playlists");
    if (it_playlists != input.end())
    {
        for (auto it : *it_playlists)
        {
            Playlist playlist;
            from_json(it, playlist);
            if (playlists.count(playlist.id))
            {
                std::cerr << "Duplicated playlist element found in input file " << playlist.id << endl;
                return 1;
            }

            playlists[playlist.id] = playlist;
        }
    }

    return 0;
}

// Validate the data of the change file
// To simplify the question, change file allows only one type of change. 
// It validates 1) For removed playlist, playlist id exist
// 2) For updated playlist exist, song id and playlist id exist
// 3) For newly added playlist, user id and song id exist and playlist id does not exist
int ValidateChangeFile(
    json& input,
    unordered_map<string, Playlist>& playlists,
    unordered_map<string, User>& users,
    unordered_map<string, Song>& songs)
{
    int changes = 0;
    // parse removedplaylists elements
    auto const it_removedplaylists = input.find("removedplaylists");
    if (it_removedplaylists != input.end())
    {
        changes++;
        for (auto it : *it_removedplaylists)
        {
            Removedplaylist removedplaylist;
            from_json(it, removedplaylist);
            if (!playlists.count(removedplaylist.id))
            {
                std::cerr << "The playlist to be removed does not exist " << removedplaylist.id << endl;
                return 1;
            }

            playlists.erase(removedplaylist.id);
        }
    }

    // parse updatedplaylists elements
    auto const it_updatedplaylists = input.find("updatedplaylists");
    if (it_updatedplaylists != input.end())
    {
        if (changes == 1)
        {
            std::cerr << "More than one type of changes exists in the change file " << endl;
            return 1;
        }

        changes++;

        for (auto it : *it_updatedplaylists)
        {
            UpdatedPlaylist updatedplaylist;
            from_json(it, updatedplaylist);
            if (!playlists.count(updatedplaylist.id))
            {
                std::cerr << "The playlist to be updated does not exist " << updatedplaylist.id << endl;
                return 1;
            }
            updatedplaylist.song_ids = it["song_ids"].get<vector<string>>();
            for (auto songid : updatedplaylist.song_ids)
            {
                if (!songs.count(songid))
                {
                    std::cerr << "The song to be added does not exist " << songid << endl;
                    return 1;
                }

                playlists[updatedplaylist.id].song_ids.insert(songid);
            }
        }
    }

    // parse newlyaddedplaylists elements
    auto const it_newlyaddedplaylists = input.find("newlyaddedplaylists");
    if (it_newlyaddedplaylists != input.end())
    {
        if (changes == 1)
        {
            std::cerr << "More than one type of changes exists in the change file " << endl;
            return 1;
        }

        for (auto it : *it_newlyaddedplaylists)
        {
            Playlist playlist;
            from_json(it, playlist);
            if (playlists.count(playlist.id))
            {
                std::cerr << "The newly added playlist exists " << playlist.id << endl;
                return 1;
            }

            if (!users.count(playlist.user_id))
            {
                std::cerr << "The newly added playlist has non existing user_id " << playlist.user_id << endl;
                return 1;
            }

            for (auto songid : playlist.song_ids)
            {
                if (!songs.count(songid))
                {
                    std::cerr << "The newly added playlist has non existing song_id " << songid << endl;
                    return 1;
                }
            }

            playlists[playlist.id] = playlist;
        }
    }

    return 0;
}

void CreateOutput(
    json& output,
    const unordered_map<string, Playlist>& playlists,
    const unordered_map<string, User>& users,
    const unordered_map<string, Song>& songs)
{
    if (!users.empty())
    {
        for (auto entry : users)
        {
            json t;
            to_json(t, entry.second);
            output["users"].push_back(t);
        }
    }

    if (!playlists.empty())
    {
        for (auto entry : playlists)
        {
            json t;
            to_json(t, entry.second);
            output["playlists"].push_back(t);
        }
    }

    if (!songs.empty())
    {
        for (auto entry : songs)
        {
            json t;
            to_json(t, entry.second);
            output["songs"].push_back(t);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc !=4) {
        show_usage(argv[0]);
        return 1;
    }

    std::string arg = argv[1];
    if ((arg == "-h") || (arg == "--help")) {
        show_usage(argv[0]);
        return 0;
    }

    // read json file for input information, like user/song/playlist
    ifstream inputfile(argv[1]);
    json input = json::parse(inputfile);
    if (input == value_t::discarded)
    {
        cerr << "input file json parsing failed\n" << endl;
    }

    unordered_map<string, User> users;
    unordered_map<string, Playlist> playlists;
    unordered_map<string, Song> songs;
    if (ValidateInputFile(input, users, songs, playlists))
    {
        cerr << "input file parsing failed\n" << endl;
        return 1;
    }

    // read json file for changed playlist information
    ifstream changefile(argv[2]);
    json change = json::parse(changefile, nullptr /* parser_callback_t */, false /* allow_exceptions */);
    if (change == value_t::discarded)
    {
        cerr << "change file json parsing failed\n" << endl;
    }

    if (ValidateChangeFile(change, playlists, users, songs))
    {
        cerr << "change file validation failed\n" << endl;
        return 1;
    }

    json output;
    CreateOutput(output, playlists, users, songs);
    ofstream outputfile(argv[3]);
    outputfile << std::setw(4) << output << std::endl;
    return 0;
}
