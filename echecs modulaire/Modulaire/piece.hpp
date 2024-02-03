#pragma once

class Piece
{
public:
	enum Couleur
	{
		BLANC,
		NOIR
	} couleur;

	virtual char get_letter() const;
};

class Pion : Piece
{
	char get_letter() const override
	{
		return 'P';
	}
};