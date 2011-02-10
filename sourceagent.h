#ifndef SOURCEAGENT_H
#define SOURCEAGENT_H

#include "agent.h"

#include <vector>

namespace Langmuir
{

  class SourceAgent:public Agent
  {

  public:
  /**
    *  @class DrainAgent
    *  @brief A agent acting as a source for charges.
    *
    *  Depending on the potential of the source, positive or negative charges enter the simulation be emerging from the source.
    *  @date 06/07/2010
    */
    SourceAgent (World * world, unsigned int site, double potential =
                 0.0, double barrier = 0.10);

    /**
     * @brief virutal Destructor.
     */
    virtual ~ SourceAgent ();

    /**
     * @brief set potential.
     *
     * Set the potential of the source.
     */
    virtual void setPotential (double potential)
    {
      m_potential = potential;
    }

    /**
     * @brief get potential.
     *
     * Get the potential of the source.
     */
    virtual double potential ()
    {
      return m_potential;
    }

    /**
     * @brief charge of agent.
     *
     * Get the charge of this agent.  What does that even mean for a source?
     * @return charge the charge of this agent.
     */
    virtual int charge ();

    /**
     * @brief transport (from???) the source. 
     *
     * Agent chooses a site and hands off to site.
     * @return site index of the site the charge carrier was moved to.  -1 if the transport was unsucessful.
     */
    virtual unsigned int transport ();

    /**
     * @brief max charges.
     *
     * Set the maximum number of charges that can be present in the system.
     */
    void setMaxCharges (int number);

    /**
     * @brief max charges.
     * 
     * Get the max number of possible charges.
     * @return The maximum number of charges in the system.
     */
    int maxCharges ();

    /**
     * @brief remove a charge.
     *
     * Used to inform the source agent that a charge has been removed from the
     * system, allowing average charge density to be kept constant.
     */
    void decrementCharge ();

    /**
     * @brief add a charge.
     *
     * Used to inform the source agent that a charge has been added to the
     * system, allowing average charge density to be kept constant.
     */
    void incrementCharge ();

  private:
    /**
      * @brief potential.
      *
      * Potential of the source.
      */
    double m_potential;

    /**
      * @brief Barrier.
      *
      * Not sure what this is.
      */
    double m_pBarrier;

    /**
      * @brief Charges.
      *
      * Number of charges injected.
      */
    int m_charges;

    /**
      * @brief Max Charges.
      *
      * Maximum number of charges.
      */
    int m_maxCharges;
  };

  inline void SourceAgent::setMaxCharges (int number)
  {
    m_maxCharges = number;
  }

  inline int SourceAgent::maxCharges ()
  {
    return m_maxCharges;
  }

  inline void SourceAgent::decrementCharge ()
  {
    if (m_charges > 0)
      --m_charges;
  }

  inline void SourceAgent::incrementCharge ()
  {
    ++m_charges;
  }

}                                // End namespace Langmuir

#endif
