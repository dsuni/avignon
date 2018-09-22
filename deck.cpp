#include "deck.h"

Deck::Deck(QString *base) : baseDir(base) {}

Deck::~Deck() {
	for (int i = tiles.size() - 1; i >= 0; --i) {
		delete tiles.at(i);
		tiles.at(i) = NULL;
	}
	for (map<QString, QSvgRenderer*>::iterator it = baseImages.begin(); it != baseImages.end(); ++it) {
		delete it->second;
	}
}

void Deck::loadTiles(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		cerr << "File not found: " << filename.toStdString() << endl;
		return;
	}
	Tile *tile;
	TileImage *image;
	QXmlStreamReader reader;
	bool subs[16];
	fill_n(subs, 16, false);
	bool ok;
	reader.setDevice(&file);
	reader.readNext();
	while (!reader.isEndDocument()) {
		QString name = reader.name().toString();
		QXmlStreamAttributes attrs = reader.attributes();
		ImageLayer *layer;
		if (name == "tile") {
			// We've reached a <tile> element, and create a new tile.
			if (reader.isStartElement()) {
				tile = new Tile(); // Deleted in destructor or upon failed sanity check
				image = tile->getImageContainer();
			}
			// We've reached a </tile> element, and can push the tile into the deck (after sanity check).
			else if (reader.isEndElement()) {
				bool good = true;
				for (int i = 0; i < 16; ++i) {
					if (!subs[i]) {
						cerr << "A tile must contain 16 subtiles. Failed on " << i << " Check your XML file." << endl;
						good = false;
						break;
					}
				}
				if (good) {
					tiles.push_back(tile);
				}
				else {
					delete tile;
				}
				fill_n(subs, 16, false);
			}
		}
		else if (name == "graphics" && reader.isStartElement()) {
			for (int i = 0; i < attrs.size(); ++i) {
				QString attrName = attrs.at(i).name().toString();
				QString attrVal = attrs.at(i).value().toString();
				if (attrName == "img") {
					QSvgRenderer *rend;
					// Search for the image - we may already have loaded it.
					map<QString, QSvgRenderer*>::const_iterator it = baseImages.find(attrVal);
					if (it == baseImages.end()) {
						// If not, load it now.
						QString tempStr(*baseDir);
						tempStr.append("/");
						tempStr.append(attrVal);
						rend = new QSvgRenderer(tempStr); // Deleted in destructor
						baseImages[attrVal] = rend;
					}
					else {
						rend = it->second;
					}
					layer = new ImageLayer(); // Will be handed to tileimage, and deleted there
					layer->renderer = rend;
					image->addImageLayer(layer);
				}
				else if (attrName == "position") {
					QStringList strList = attrVal.split(",");
					int x = strList.value(0).toInt(&ok);
					if (!ok) {
						cerr << "Casting position X " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
					int y = strList.value(1).toInt(&ok);
					if (!ok) {
						cerr << "Casting position Y " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
					layer->x = x;
					layer->y = y;
				}
				else if (attrName == "rotation") {
					int rot = attrVal.toInt(&ok);
					if (!ok) {
						cerr << "Casting rotation " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
					layer->rotation = rot;
				}
				else if (attrName == "scale") {
					QStringList strList = attrVal.split("%");
					double scale = strList.value(0).toDouble(&ok);
					if (!ok) {
						cerr << "Casting scale " << attrs.at(i).value().toString().toStdString() << " to double failed. Check your XML file." << endl;
					}
					scale /= 100;
					layer->scale = scale;
				}
				else if (attrName == "stationary") {
					layer->stationary = (attrVal.toLower() == "true");
				}
			}
		}
		else if (name == "subtile" && reader.isStartElement()) {
			int pos = -1;
			Terrain t = undefined;
			TerrainAddon ta = none;
			for (int i = 0; i < attrs.size(); ++i) {
				QString attrName = attrs.at(i).name().toString();
				QString attrVal = attrs.at(i).value().toString();
				if (attrName == "position") {
					pos = attrs.at(i).value().toInt(&ok);
					if (!ok) {
						cerr << "Casting position " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
				}
				else if (attrName == "terrain") {
					t = (terrainMap()[attrs.at(i).value().toString().toStdString()]);
				}
				else if (attrName == "addon") {
					ta = (terrainAddonMap()[attrs.at(i).value().toString().toStdString()]);
				}
			}
			if (pos < 0 || pos > 15 || t == undefined) {
				cerr << "Position must be in range 0-15, and terrain must not be undefined. Check your XML file." << endl;
			}
			else {
				tile->addSubtile(new Subtile(t, ta, tile, pos), pos);
				subs[pos] = true;
			}
		}
		else if (name == "connection" && reader.isStartElement()) {
			int to = -1;
			int from = -1;
			for (int i = 0; i < attrs.size(); ++i) {
				QString attrName = attrs.at(i).name().toString();
				if (attrName == "from") {
					from = attrs.at(i).value().toInt(&ok);
					if (!ok) {
						cerr << "Casting " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
				}
				else if (attrName == "to") {
					to = attrs.at(i).value().toInt(&ok);
					if (!ok) {
						cerr << "Casting " << attrs.at(i).value().toString().toStdString() << " to integer failed. Check your XML file." << endl;
					}
				}
			}
			if (from < 0 || from > 15 || to < 0 || to > 15) {
				cerr << "Values for 'from' and 'to' must be in range 0-15. Check your XML file." << endl;
			}
			else {
				tile->connectSubtiles(from, to);
			}
		}
		reader.readNext();
	}
}

void Deck::shuffleDeck() {
	shuffleDeck(0, tiles.size() - 1);
}

void Deck::shuffleDeck(int from) {
	shuffleDeck(from, tiles.size() - 1);
}

void Deck::shuffleDeck(int from, int to) {
	if (from >= to || to >= tiles.size()) {
		return;
	}
	srand(time(NULL));
	int random;
	for (int i = to; i >= from; --i) {
		random = rand() % (to - from);
		swapTiles(i, random + from);
	}
}

void Deck::eraseTile(int num) {
	if (num < 0 || num >= tiles.size()) {
		return;
	}
	delete tiles.at(num);
	tiles.erase(tiles.begin() + num);
}

void Deck::swapTiles(int num1, int num2) {
	Tile *temp = tiles.at(num1);
	tiles.at(num1) = tiles.at(num2);
	tiles.at(num2) = temp;
}

void Deck::mixIntoDeck(int num) {
	srand(time(NULL));
	int diff = tiles.size() - num - 1;
	int random = (rand() % diff) + 1;
	swapTiles(num, random);
}

int Deck::getSize() {
	return tiles.size();
}

Tile* Deck::getTile(int position) {
	return tiles.at(position);
}

