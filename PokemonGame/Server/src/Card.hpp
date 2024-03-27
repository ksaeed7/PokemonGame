#ifndef CARD_COMMAND_HPP
#define CARD_COMMAND_HPP
#include <string>

class Card {
public:
    /*Card(int id, const std::string& name, const std::string& type,
        const std::string& rarity, int count, int owner_id)
        : id_(id), name_(name), type_(type),
        rarity_(rarity), count_(count),
        owner_id_(owner_id), owner_name_("") {}*/
    Card(int id, const std::string& name, const std::string& type,
        const std::string& rarity, int count, int owner_id, const std::string& owner_name)
        : id_(id), name_(name), type_(type),
        rarity_(rarity), count_(count),
        owner_id_(owner_id), owner_name_(owner_name) {}
    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getType() const { return type_; }
    const std::string& getRarity() const { return rarity_; }
    int getCount() const { return count_; }
    int getOwnerId() const { return owner_id_; }
    const std::string& getOwnerName() const { return owner_name_; }

private:
    int id_;
    std::string name_;
    std::string type_;
    std::string rarity_;
    int count_;
    int owner_id_;
    std::string owner_name_;
};

#endif