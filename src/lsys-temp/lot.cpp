#include "lot.h"

#include "units.h"
#include "point.h"
#include "polygon.h"
#include "vector.h"
#include "block.h"

Lot::Lot()
{
  initialize();
}

Lot::Lot(Block* parentBlock, Polygon const& area)
{
  initialize();
  setParent(parentBlock);
  setAreaConstraints(area);
}

Lot::Lot(Lot const& source)
  : Area(source)
{
  initialize();
}

void Lot::initialize()
{
}

Lot& Lot::operator=(Lot const& source)
{
  Area::operator=(source);

  return *this;
}

Lot::~Lot()
{
  freeMemory();
}

void Lot::freeMemory()
{
}
