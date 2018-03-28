/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdexcept>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanelFactory.hpp"
#include "FilterPanels.hpp"

using namespace mdl;


FilterPanelFactory::FilterPanelFactory(int max_frame, int frame_width, int frame_height)
  : max_frame_(max_frame)
  , frame_width_(frame_width)
  , frame_height_(frame_height)
{
}


FilterPanel* FilterPanelFactory::create(int start_frame, fg::Filter* filter)
{
  switch (filter->type()) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull(start_frame, max_frame_);

  case fg::FilterType::CUT:
    return new FilterPanelCut(start_frame, max_frame_);

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(start_frame, max_frame_,
                                 dynamic_cast<fg::DelogoFilter*>(filter),
                                 frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(start_frame, max_frame_,
                                  dynamic_cast<fg::DrawboxFilter*>(filter),
                                  frame_width_, frame_height_);

  case fg::FilterType::REVIEW:
    return new FilterPanelReview(start_frame, max_frame_);

  default:
    return nullptr;
  }
}


FilterPanel* FilterPanelFactory::create(int start_frame, fg::FilterType type)
{
  switch (type) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull(start_frame, max_frame_);

  case fg::FilterType::CUT:
    return new FilterPanelCut(start_frame, max_frame_);

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(start_frame, max_frame_,
                                 frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(start_frame, max_frame_,
                                  frame_width_, frame_height_);

  case fg::FilterType::REVIEW:
    return new FilterPanelReview(start_frame, max_frame_);

  default:
    return nullptr;
  }
}


FilterPanel* FilterPanelFactory::convert(int start_frame, fg::Filter* original, fg::FilterType new_type)
{
  if (is_rectangular(original->type()) && is_rectangular(new_type)) {
    FilterPanel* panel = create(start_frame, new_type);
    fg::RectangularFilter* rectangular = dynamic_cast<fg::RectangularFilter*>(original);
    panel->set_rectangle({.x = (gdouble) rectangular->x(), .y = (gdouble) rectangular->y(),
                          .width = (gdouble) rectangular->width(), .height = (gdouble) rectangular->height()});
    return panel;
  }

  if (is_no_parameters(new_type) || is_no_parameters(original->type())) {
    return create(start_frame, new_type);
  }

  throw std::invalid_argument("Unsupported conversion");
}


bool FilterPanelFactory::is_no_parameters(fg::FilterType type)
{
  return type == fg::FilterType::NO_OP
      || type == fg::FilterType::CUT
      || type == fg::FilterType::REVIEW;
}


bool FilterPanelFactory::is_rectangular(fg::FilterType type)
{
  return type == fg::FilterType::DELOGO || type == fg::FilterType::DRAWBOX;
}

