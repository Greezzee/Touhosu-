#pragma once
namespace soundManager
{
	static sf::SoundBuffer bulletShoot;
	static sf::Sound shoot;
	static sf::Music music;

	static void init() {
		bulletShoot.loadFromFile("Sounds/bulletShootSound.wav");
	}
	static void bulletShootSound() {
		shoot.setBuffer(bulletShoot);
		//shoot.play();
	}
};

