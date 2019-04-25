#pragma once
#define BOOST_ALL_NO_LIB
#include "Common.h"
#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Wall.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_text_oarchive.hpp>
#include <boost/archive/polymorphic_text_iarchive.hpp>
#include <boost/variant.hpp>

//https://stackoverflow.com/questions/25889668/how-to-create-a-interface-for-serialization-in-boost-serialization

using PolyArchive = boost::variant<
	boost::archive::polymorphic_oarchive&,
	boost::archive::polymorphic_iarchive&
>;

namespace /*detail*/ {
	template <typename F> struct wrap_visitor : boost::static_visitor<> {

		wrap_visitor(F const& f) : f_(f) { }
		wrap_visitor(F&& f) : f_(std::move(f)) { }

		template<typename... T> void operator()(T&&... t) const {
			f_(std::forward<T>(t)...);
		}

	private:
		F f_;
	};

	template <typename F>
	wrap_visitor<F> make_visitor(F&& f) {
		return std::forward<F>(f);
	}
}

struct /*abstract*/ Serializable {
	virtual void serialize(PolyArchive, unsigned) = 0;
};
enum PlayerCommands {
	FORWARD = 1 << 0,
	BACKWARD = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	JUMP = 1 << 4,
	WALL = 1 << 5,
	SWING = 1 << 6
};

struct GameState {
	vector<Player *> players;
	vector<Ball *> balls;
	vector<Wall *> walls;
	vector<GameObject *> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;
};

struct GameStateNet {
	vector<Player> players;
	vector<Ball> balls;
	vector<Wall> walls;
	vector<GameObject> gameObjects;
	long timeLeft;
	tuple<int, int> score;
	bool in_progress;

	//void serialize(PolyArchive ar, const unsigned int version)
	//{
	//	boost::apply_visitor(make_visitor([=](auto& ar) {
	//		ar & players;
	//		ar & balls;
	//		ar & walls;
	//		ar & gameObjects;
	//		ar & timeLeft;
	//		ar & score.first;
	//		ar & score.second;
	//		ar & in_progress;
	//	}), ar);
	//}
};

struct PlayerInputs :public Serializable {
	int inputs;
	int id;

	void serialize(PolyArchive ar, const unsigned int version)
	{
		boost::apply_visitor(make_visitor([=](auto& ar) {
			ar & inputs;
			ar & id;
		}), ar);
	}
};